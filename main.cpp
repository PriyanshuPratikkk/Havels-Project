#include <bits/stdc++.h>
using namespace std;

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

const double EARTH_RADIUS = 6371.0;

// latency factors
const double BASE_LATENCY_MS = 5.0;     // base latency for processing
const double LATENCY_FACTOR = 0.005;    // ms per km (simulated)

// Convert degrees to radians
inline double toRadians(double degree) {
    return degree * M_PI / 180.0;
}

// Haversine formula to calculate distance
double haversine(double lat1, double lon1, double lat2, double lon2) {
    lat1 = toRadians(lat1);
    lon1 = toRadians(lon1);
    lat2 = toRadians(lat2);
    lon2 = toRadians(lon2);

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = pow(sin(dlat / 2), 2) +
               cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return EARTH_RADIUS * c;
}

struct Server {
    string name;
    double lat, lon;
    Server(string n, double la, double lo) : name(n), lat(la), lon(lo) {}
};

struct Router {
    vector<Server> servers;

    // Stats tracking
    double totalLatency = 0;
    double minLatency = DBL_MAX;
    double maxLatency = 0;
    int requestCount = 0;

    void addServer(const string &name, double lat, double lon) {
        servers.emplace_back(name, lat, lon);
        cout << GREEN << "✅ Server '" << name << "' added successfully at (" 
             << lat << ", " << lon << ")" << RESET << "\n";
    }

    string routeRequest(int requestID, const string &originName, double reqLat, double reqLon) {
        if (servers.empty()) {
            cout << RED << "❌ No servers available to handle the request!" << RESET << "\n";
            return "";
        }

        string bestServer;
        double bestLatency = DBL_MAX;
        double minDist = DBL_MAX;

        cout << CYAN << "\n===== Processing Request #" << requestID << " =====" << RESET << "\n";
        cout << CYAN << "Origin: " << originName 
             << " (" << reqLat << ", " << reqLon << ")" << RESET << "\n";

        cout << YELLOW << "Evaluating all servers for lowest latency..." << RESET << "\n";

        // First pass: find minimum
        for (auto &s : servers) {
            double dist = haversine(reqLat, reqLon, s.lat, s.lon);
            double latency = BASE_LATENCY_MS + dist * LATENCY_FACTOR;
            if (latency < bestLatency) {
                bestLatency = latency;
                minDist = dist;
                bestServer = s.name;
            }
        }

        // Second pass: print all with highlight
        for (auto &s : servers) {
            double dist = haversine(reqLat, reqLon, s.lat, s.lon);
            double latency = BASE_LATENCY_MS + dist * LATENCY_FACTOR;
            if (s.name == bestServer) {
                cout << GREEN << "  -> " << s.name << ": "
                     << fixed << setprecision(2) << dist << " km, "
                     << latency << " ms (Fastest)" << RESET << "\n";
            } else {
                cout << "     " << s.name << ": "
                     << fixed << setprecision(2) << dist << " km, "
                     << latency << " ms\n";
            }
        }

        cout << GREEN << ">> Final Routing Decision: Request #" << requestID 
             << " served by " << bestServer 
             << " [" << fixed << setprecision(2) << bestLatency << " ms]" << RESET << "\n";
        cout << CYAN << "==============================================" << RESET << "\n";

        // Update stats
        totalLatency += bestLatency;
        minLatency = min(minLatency, bestLatency);
        maxLatency = max(maxLatency, bestLatency);
        requestCount++;

        return bestServer;
    }

    void printSummary() {
        cout << CYAN << "\n========= Performance Summary =========" << RESET << "\n";
        if (requestCount == 0) {
            cout << RED << "No requests processed." << RESET << "\n";
            return;
        }
        cout << "Total Requests: " << requestCount << "\n";
        cout << "Average Response Time: " 
             << fixed << setprecision(2) << (totalLatency / requestCount) << " ms\n";
        cout << "Fastest Response Time: " 
             << fixed << setprecision(2) << minLatency << " ms\n";
        cout << "Slowest Response Time: " 
             << fixed << setprecision(2) << maxLatency << " ms\n";
        cout << CYAN << "=========================================" << RESET << "\n";
    }
};

int main() {
    Router router;
    int choice, reqId = 1;

    // Start with some default servers
    router.addServer("New York", 40.7128, -74.0060);
    router.addServer("London", 51.5074, -0.1278);
    router.addServer("Tokyo", 35.6895, 139.6917);

    while (true) {
        cout << CYAN << "\n--- Router Console ---" << RESET << "\n";
        cout << "1. Add new server\n";
        cout << "2. Send new request\n";
        cout << "3. Show performance summary\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            string name;
            double lat, lon;
            cout << "Enter server name: ";
            cin >> ws;
            getline(cin, name);
            cout << "Enter latitude: ";
            cin >> lat;
            cout << "Enter longitude: ";
            cin >> lon;
            router.addServer(name, lat, lon);
        } 
        else if (choice == 2) {
            string origin;
            double lat, lon;
            cout << "Enter request origin name: ";
            cin >> ws;
            getline(cin, origin);
            cout << "Enter latitude: ";
            cin >> lat;
            cout << "Enter longitude: ";
            cin >> lon;
            router.routeRequest(reqId++, origin, lat, lon);
        } 
        else if (choice == 3) {
            router.printSummary();
        } 
        else if (choice == 4) {
            cout << GREEN << "Exiting router service. Goodbye!" << RESET << "\n";
            break;
        } 
        else {
            cout << RED << "Invalid choice. Try again." << RESET << "\n";
        }
    }

    return 0;
}
