// ============================================================
//  statistics.cpp
//  Student result storage and statistical calculations.
// ============================================================

#include "statistics.h"
using namespace std;

StudentResult results[MAX_STUDENTS];
int           totalResults = 0;

void storeResult(const StudentResult& result) {
    if (totalResults < MAX_STUDENTS)
        results[totalResults++] = result;
}

double getAverageScore() {
    if (totalResults == 0) return 0.0;
    double total = 0.0;
    for (int i = 0; i < totalResults; i++)
        total += results[i].percentage;
    return total / totalResults;
}

int getHighestScoreIndex() {
    if (totalResults == 0) return -1;
    int best = 0;
    for (int i = 1; i < totalResults; i++)
        if (results[i].percentage > results[best].percentage) best = i;
    return best;
}

int getLowestScoreIndex() {
    if (totalResults == 0) return -1;
    int worst = 0;
    for (int i = 1; i < totalResults; i++)
        if (results[i].percentage < results[worst].percentage) worst = i;
    return worst;
}

double getCategoryPassRate(int categoryIndex) {
    int totalCorrect = 0, totalAnswered = 0;
    for (int i = 0; i < totalResults; i++) {
        totalCorrect  += results[i].categoryCorrect[categoryIndex];
        totalAnswered += results[i].categoryTotal[categoryIndex];
    }
    return (totalAnswered > 0) ? (double)totalCorrect / totalAnswered * 100.0 : 0.0;
}
