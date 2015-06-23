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

#include "TrustedAuthority.h"
#include "CoreEvaluation_m.h"

Define_Module(TrustedAuthority);

void TrustedAuthority::initialize() {
    // TODO - Generated method body
}

void TrustedAuthority::handleMessage(cMessage *msg) {
    // TODO - Generated method body
    CoreEvaluation* eval = check_and_cast<CoreEvaluation*>(msg);
    IPList core = eval->getCore();
    std::cout << "Core: ";
    for (auto it = core.begin(); it != core.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << endl;
    double dropEstimation = eval->getDropEstimation();
    std::cout << "Drop estimation: " << dropEstimation << endl;
    delete msg;
}
