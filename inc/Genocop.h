#ifndef GENOCOP_H
#define GENOCOP_H

#include <valarray>
#include <functional>
#include <vector>
#include <random>
#include <chrono>
#include <stdint.h>

#include "ObjectiveFunction.h"

class Genocop
{
public:
    typedef std::valarray<double> Vector;

    struct Options
    {
        uint32_t populationCount = 100;
        uint32_t parentsCount = 35;
        uint32_t maxIters = 1000;

        // =============================================
        // ================= Selection =================
        // =============================================
        struct
        {   
            int size = 4; // larger size -> larger selection pressure
            double p = 0.9; // probability that the best in the tournament wins. Set to 1 for 100% deterministic tournament
        } tournament;

        // =============================================
        // ============= Genetic operators =============
        // =============================================

        // Elitism: copy the best individuals directly to the next generation
        // They can still participate in tournaments
        uint32_t eliteChildrenCount = 0; 

        struct 
        {
            double totalProbability = 0.3;
            double pClassic = 0.4;
            double pLinear = 0.3;
            double pHeuristic = 0.3;
            double heuristicRangeMult = 2.0;
        } crossover;
        
        struct 
        {
            double pFull = 0.005;
            double pFine = 0.3;
            double fineMutationMin = 1e-5;
            double fineMutationMax = 0.15;
        } mutatation;
    };

    Genocop(const uint32_t vectorSize, ObjectiveFunction objective, 
            const Vector xMin, const Vector xMax);

    double run(Vector & outSolution, Genocop::Options options);
    
private:
    // Represents an individual and his score
    struct Score
    {
        Vector x;
        double value;

        bool operator<(const Score & other) const
        {
            return value < other.value;
        }
    };

    ObjectiveFunction objFunction;
    const size_t vectorSize;

    // =============================================
    // ========== Random value generators ==========
    // =============================================

    std::default_random_engine randomEngine;
    std::uniform_real_distribution<double> pRng;  // probability: [0, 1]
    std::uniform_real_distribution<double> mRng;  // mutation: [-1, 1]
    std::uniform_int_distribution<uint32_t> cRng; // crossover index: [1, vectorSize - 1]

    Vector xMin;
    Vector xMax;
    // Derivative values: random values for mutation will be generated in the [-1, 1] range for simplicity
    // These vectors represent the mapping [-1, 1] -> [xMin[i], xMax[i]]
    // offsetX is the average of min and max
    Vector offsetX;
    Vector scaleX; 

    // used by selectParents and create children - avoid allocation every time
    std::vector<uint32_t> scoreIdx;

    // Selects outParents.size() individuals from scores (with possible repetition) based on values
    // using tournaments
    // - tournamentSize: how many individuals to participate in the tournament. Higher values => more selection pressure
    // - tournamentP: probability that the best individual wins a tournament
    void selectParents(const std::vector<Score> & scores, std::vector<Score> & outParents,
                       const uint32_t tournamentSize, const double tournamentP);

    // Create children from parents using:
    // - elitism: copy the best parents directly into the output
    // - crossovers
    // - direct copying (if crossovers do not produce enough children)
    //
    // For each child that is not elite, run a mutation check
    void createChildren(const std::vector<Score> & scores, std::vector<Score> & parents,
                        std::vector<Vector> & outChildren, const Genocop::Options & options,
                        const uint32_t iter);

    // =============================================
    // ============= Genetic operators =============
    // =============================================

    void classicCrossover(const Score & parent0, const Score & parent1,
                          Vector & outChild0, Vector & outChild1);

    void linearCrossover(const Score & parent0, const Score & parent1, double alpha,
                         Vector & outChild);

    // Linear crossover in the direction worse -> better    
    void heuristicCrossover(const Score & parent0, const Score & parent1, double alpha,
                            Vector & outChild);

    void mutation(Vector & x, const double pFull, const double pFine, const double fineRange);

    // =============================================
    // =============== RNG functions ===============
    // =============================================

    // [0, 1]
    inline double getProbability()
    {
        return this->pRng(this->randomEngine);
    }

    // [-1, 1]
    inline double getMutation()
    {
        return this->mRng(this->randomEngine);
    }

    // [1, vectorSize - 1]
    inline uint32_t getCrossoverIdx()
    {
        return this->cRng(this->randomEngine);
    }
};

#endif