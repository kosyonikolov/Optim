#include "Genocop.h"

#include <algorithm>
#include <iostream>

Genocop::Genocop(const uint32_t vectorSize, ObjectiveFunction objective, 
            const Vector xMin, const Vector xMax) : 
                objFunction(objective), vectorSize(vectorSize),
                pRng(0, 1), mRng(-1, 1), cRng(1, vectorSize - 1), 
                xMin(xMin), xMax(xMax)
{
    this->offsetX.resize(vectorSize);
    this->scaleX.resize(vectorSize);

    // compute scales and offsets for subsequent runs
    for (uint32_t i = 0; i < vectorSize; i++)
    {
        offsetX[i] = 0.5 * (xMin[i] + xMax[i]);
        scaleX[i] = 0.5 * (xMax[i] - xMin[i]);
    }

    // seed the rng
    this->randomEngine.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

double Genocop::run(Vector & outSolution, Genocop::Options options)
{
    const uint32_t VECTOR_SIZE = this->vectorSize;
    const uint32_t POPULATION_COUNT = options.populationCount;
    const uint32_t PARENTS_COUNT = options.parentsCount;
    const uint32_t MAX_ITERS = options.maxIters;

    // Sanity check (make options sensible)
    {
        if (VECTOR_SIZE < 2)
        {
            // only one element: classic crossover makes no sense
            options.crossover.pClassic = 0;
        }

        // normalize crossover probabilities sum
        auto & cross = options.crossover;
        if (cross.totalProbability > 0)
        {
            const double pSum = cross.pClassic + cross.pHeuristic + cross.pLinear;
            if (pSum <= 0)
            {
                // Can't fix
                throw std::runtime_error("Zero crossovers probability sum!");
            }

            cross.pClassic /= pSum;
            cross.pHeuristic /= pSum;
            cross.pLinear /= pSum;
        }        
        else if (cross.totalProbability < -1e-5)
        {
            throw std::runtime_error("Negative crossover probability!");
        }
    }

    // Allocate just once - save time
    std::vector<Vector> population(POPULATION_COUNT);
    std::vector<Score> parents(PARENTS_COUNT);
    std::vector<Score> scores(POPULATION_COUNT);

    // allocate parents
    for (uint32_t i = 0; i < PARENTS_COUNT; i++)
    {
        parents[i].x.resize(VECTOR_SIZE); 
    }

    // create first population
    for (uint32_t i = 0; i < POPULATION_COUNT; i++)
    {
        // allocate scores and individuals        
        scores[i].x.resize(VECTOR_SIZE);
        Vector & individual = population[i];
        individual.resize(VECTOR_SIZE);
        for (uint32_t j = 0; j < VECTOR_SIZE; j++)
        {
            // first generation is random
            individual[j] = this->offsetX[j] + this->scaleX[j] * getMutation();
        }
    }

    double bestScore = 1e+99;
    uint32_t bestIdx = 0;
    double averageScore = 0;

    auto calculateScores = [&]()
    {
        averageScore = 0;

        // calculate scores
        for (uint32_t j = 0; j < POPULATION_COUNT; j++)
        {
            scores[j].x = population[j];
            scores[j].value = this->objFunction(population[j]); 

            if (scores[j].value < bestScore)
            {
                bestScore = scores[j].value;
                bestIdx = j;
            }
            averageScore += scores[j].value;
        }

        averageScore /= POPULATION_COUNT;

        if (this->callback != 0)
            callback(scores);
    };

    // main optimization loop
    for (uint32_t i = 0; i < MAX_ITERS; i++)
    {
        calculateScores();
        std::cout << i << "\t" << bestScore << "\t" << averageScore << "\n";

        selectParents(scores, parents, options.tournament.size, options.tournament.p);
        createChildren(scores, parents, population, options, i);
    }

    calculateScores();
    std::cout << MAX_ITERS << "\t" << bestScore << "\t" << averageScore << "\n";

    outSolution = scores[bestIdx].x;
    return bestScore;
}

void Genocop::selectParents(const std::vector<Score> & scores, std::vector<Score> & outParents,
                            const uint32_t tournamentSize, const double tournamentP)
{
    const uint32_t SCORES_COUNT = scores.size();

    // check if the index vector is the proper size
    {        
        if (scoreIdx.size() != SCORES_COUNT)
        {
            scoreIdx.resize(SCORES_COUNT);
            for (uint32_t i = 0; i < SCORES_COUNT; i++)
                scoreIdx[i] = i;
        }
    }
    
    std::uniform_int_distribution<uint32_t> idxRng(0, 137137137);
    auto & rng = this->randomEngine;

    // comparison function
    auto scoreIdxCompare = [&](uint32_t a, uint32_t b) -> bool
    {
        return scores[a] < scores[b];
    };

    auto runTournament = [&]() -> uint32_t
    {
        // shuffle the first tournamentSize indices
        for (uint32_t i = 0; i < tournamentSize; i++)
        {
            // choose an element to swap with that has a higher idx than the current element
            const uint32_t swapIdx = idxRng(rng) % (SCORES_COUNT - i) + i;
            std::swap(scoreIdx[i], scoreIdx[swapIdx]);
        }

        // sort the first elements according to their representitive values
        std::sort(scoreIdx.begin(), scoreIdx.begin() + tournamentSize, scoreIdxCompare);

        // decide the winner
        for (uint32_t i = 0; i < tournamentSize - 1; i++)
        {
            const double p = getProbability();
            if (p < tournamentP)
            {
                return scoreIdx[i];
            }
        }

        // if we are here then none of the previous tournament participants won
        // so the last participant is the winner
        return scoreIdx[tournamentSize - 1];
    };

    // finally select parents
    for (uint32_t i = 0; i < outParents.size(); i++)
    {
        const uint32_t winnerIdx = runTournament();
        outParents[i] = scores[winnerIdx];
    }
}


void Genocop::createChildren(const std::vector<Score> & scores, std::vector<Score> & parents,
                             std::vector<Vector> & outChildren, const Genocop::Options & options,
                             const uint32_t iter)
{
    const uint32_t PARENT_COUNT = parents.size();
    const uint32_t CHILDREN_COUNT = outChildren.size();

    uint32_t childIdx = 0;

    // create children via elitism
    if (options.eliteChildrenCount > 0)
    {
        auto compareIdx = [&](uint32_t a, uint32_t b)
        {
            return scores[a] < scores[b];
        };

        // find the elite, O(N) time
        std::nth_element(scoreIdx.begin(), scoreIdx.begin() + options.eliteChildrenCount, scoreIdx.end(), compareIdx);
        // copy to output
        for (uint32_t i = 0; i < options.eliteChildrenCount; i++)
        {
            outChildren[i] = scores[scoreIdx[i]].value;
        }

        childIdx = options.eliteChildrenCount;
    }

    // children from this position onwards will be affected by mutation
    const uint32_t mutationStartIdx = childIdx;

    std::uniform_int_distribution<uint32_t> idxRng(0, PARENT_COUNT - 1);
    auto & rng = this->randomEngine;

    // ================================================================
    // ================ Create children via crossovers ================
    // ================================================================

    // prepare thresholds
    const double pCrossover = options.crossover.totalProbability;
    const double pClassic = options.crossover.pClassic;
    const double pLinear = pClassic + options.crossover.pLinear;

    for (uint32_t i = 0; i < parents.size() && childIdx < CHILDREN_COUNT; i++)
    {
        double p = getProbability();
        if (p > pCrossover)
            continue;

        // select parents
        Score parent0, parent1;
        {
            uint32_t idx0 = idxRng(rng);
            uint32_t idx1 = idxRng(rng);
            if (idx1 == idx0)
            {
                // avoid same parent
                idx1 = (idx1 + 1) % PARENT_COUNT;
            }

            parent0 = parents[idx0];
            parent1 = parents[idx1];
        }
        // allocate children
        Vector child0 = parent0.x;
        Vector child1 = parent1.x;

        // select type
        p = getProbability();
        if (p <= pClassic)
        {
            classicCrossover(parent0, parent1, child0, child1);
            outChildren[childIdx++] = child0;
            if (childIdx < CHILDREN_COUNT)
                outChildren[childIdx++] = child1;
        }
        else if (p <= pLinear)
        {
            const double alpha = getProbability();
            linearCrossover(parent0, parent1, alpha, child0);
            outChildren[childIdx++] = child0;
        }
        else // heuristic
        {
            const double alpha = getProbability() * options.crossover.heuristicRangeMult;
            heuristicCrossover(parent0, parent1, alpha, child0);
            outChildren[childIdx++] = child0;
        }        
    }

    // ================================================================
    // ============== Copy parents into remaining slots ===============
    // ================================================================

    for (; childIdx < CHILDREN_COUNT; childIdx++)
    {
        // parents already have duplicates based on function value
        // so it is enough to sample uniformly to give better parents more children
        uint32_t parentIdx = idxRng(rng);
        outChildren[childIdx] = parents[parentIdx].x;
    }

    // ================================================================
    // =================== Mutate created children ====================
    // ================================================================

    // determine fine mutation range
    const auto & m = options.mutatation;
    const double fineMutationRange = m.fineMutationMin + (m.fineMutationMax - m.fineMutationMin) * (1 - double(iter) / options.maxIters);

    for (uint32_t i = mutationStartIdx; i < CHILDREN_COUNT; i++)
    {
        double p = getProbability();
        if (p <= m.pFine)
        {
            fineRangeMutation(outChildren[i], fineMutationRange);
        }

        fullRangeMutation(outChildren[i], m.pFull);     
    }
}

void Genocop::classicCrossover(const Score & parent0, const Score & parent1,
                               Vector & outChild0, Vector & outChild1)
{
    const uint32_t VECTOR_SIZE = this->vectorSize;
    const uint32_t crossIdx = getCrossoverIdx();

    uint32_t i = 0;
    for (; i < crossIdx; i++)
    {
        outChild0[i] = parent0.x[i];
        outChild1[i] = parent1.x[i];
    }
    for (; i < VECTOR_SIZE; i++)
    {
        outChild0[i] = parent1.x[i];
        outChild1[i] = parent0.x[i];
    }
}

void Genocop::linearCrossover(const Score & parent0, const Score & parent1, double alpha,
                              Vector & outChild)
{
    outChild = (1.0 - alpha) * parent0.x + alpha * parent1.x;
}

void Genocop::heuristicCrossover(const Score & parent0, const Score & parent1, double alpha,
                                 Vector & outChild)
{
    if (parent0.value > parent1.value)
    {
        outChild = (1.0 - alpha) * parent0.x + alpha * parent1.x;
    }
    else
    {
        outChild = (1.0 - alpha) * parent1.x + alpha * parent0.x;
    }
}

void Genocop::fullRangeMutation(Vector & x, const double pFull)
{
    for (uint32_t i = 0; i < x.size(); i++)
    {
        double p = getProbability();
        if (p <= pFull)
        {
            // full range mutation: set element to random value in range
            const double normalizedValue = getMutation();
            x[i] = offsetX[i] + scaleX[i] * normalizedValue;
        }     
    }
}

void Genocop::fineRangeMutation(Vector & x, const double range)
{
    Vector dir = x;
    getRandomDirection(dir);
    double mult = getMutation() * range;
    dir *= mult;

    x += dir;
    for (uint32_t i = 0; i < x.size(); i++)
    {
        x[i] = std::max(x[i], this->xMin[i]);
        x[i] = std::min(x[i], this->xMax[i]);
    }
}

inline void Genocop::getRandomDirection(Vector & dir)
{
    // Generate random direction in N-space as per Muller, M. E. "A Note on a Method for Generating Points Uniformly on N-Dimensional Spheres.", 1959. 

    double sumSq = 0;
    uint32_t i = 0;
    while (sumSq < 1e-5 && i < 3) // avoid too small sums
    {
        i++;
        for (uint32_t i = 0; i < dir.size(); i++)
        {
            double val = this->dRng(this->randomEngine);
            sumSq += val * val;
            dir[i] = val;
        }
    }

    sumSq = std::max(1e-5, sumSq); // we may have been very unlucky
    const double mult = 1.0 / std::sqrt(sumSq);
    dir *= mult;
}