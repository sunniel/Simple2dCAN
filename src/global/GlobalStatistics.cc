//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "GlobalStatistics.h"

Define_Module(GlobalStatistics);

GlobalStatistics::GlobalStatistics() {

}

GlobalStatistics::~GlobalStatistics() {

}

void GlobalStatistics::initialize() {
    WATCH_VECTOR(hops);
}

void GlobalStatistics::handleMessage(cMessage *msg) {
    // TODO - Generated method body
}

double meanCalculator(vector<int> list) {
    int lenght = list.size();
    if (lenght == 0)
        return 0;
    int sum = 0;
    for (int i = 0; i < lenght; i++) {
        sum = sum + list[i];
    }
    double mean = (double) sum / lenght;
    return mean;
}

void GlobalStatistics::finish() {
    cout << "Generating statistical results" << endl;

    cout << "Sample size: " << hops.size() << endl;

    int max = 0;
    int min = INT_MAX;
    for (int j = 0; j < hops.size(); j++) {
        int hop_count = hops[j];
        if (hop_count > max)
            max = hop_count;
        if (hop_count < min)
            min = hop_count;
    }
    double mean = meanCalculator(hops);
    cout << "Mean:  " << mean << "(" << mean - 1 << ") Max Value: " << max
            << "(" << max - 1 << ") Min Value: " << min << "(" << min - 1 << ")"
            << endl;
    cout << "Join failures: " << JOIN_FAILS << endl;
    cout << "Failures: " << FAILS << " Successes: " << SUCCESS << endl;
    cout << "Departure: " << DEPARTURE << " New arrival: " << ARRIVAL << endl;
    cout << "Final system size: " << GlobalNodeListAccess().get()->canSize()
            << endl;

    cout << "End of statistical results generation" << endl;
}

void GlobalStatistics::addHop(int hop) {
    hops.push_back(hop);
}

