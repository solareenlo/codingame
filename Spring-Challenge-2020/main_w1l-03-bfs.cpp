#pragma GCC optimize("-O3")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops")

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <cmath>
#include <iomanip>
#include <chrono>
#define REP(i, n) for (int i = 0; i <(n); i++)
using namespace std;
using P = pair<int, int>;
using VP = vector<P>;
using QP = queue<P>;
using VI = vector<int>;
using VVI = vector<vector<int> >;
constexpr double INF_DOUBLE = numeric_limits<double>::infinity();
constexpr int INF_INT = numeric_limits<int>::infinity();
constexpr int DX[4] = {1, 0, -1, 0};
constexpr int DY[4] = {0, 1, 0, -1};

// 幅優先探索で from から map 上の経路を作成する
inline VVI calDist(P from, P to, VVI map) {
    int height = map.size();
    int width = map[0].size();

    /* 幅優先探索の初期設定 */
    VVI dist(height, VI(width, 999)); // 各セルの最短距離 (訪れていないところは 999 としておく)
    dist[from.second][from.first] = 0; // スタートを 0 に設定
    QP que; // 「一度見た頂点」のうち「まだ訪れていない頂点」を表すキュー
    que.push(make_pair(from.first, from.second)); // スタートを push

    /* キューが空になるまで */
    while (!que.empty()) {
        P currentPos = que.front(); // キューの先頭を見る
        int x = currentPos.first;
        int y = currentPos.second;
        que.pop(); // キューから先頭を pop する

        // 隣接頂点を探索
        REP(direction, 4) {
            int nextX = x + DX[direction];
            int nextY = y + DY[direction];
            if (nextX < 0 || nextX >= width || nextY < 0 || nextY >= height)
                continue; // 場外アウトはだめ
            if (map[nextY][nextX] == 0)
                continue; // 壁はだめ
            // まだ見ていない頂点なら push
            if (dist[nextY][nextX] == 999) {
                que.push(make_pair(nextX, nextY));
                dist[nextY][nextX] = dist[y][x] + 1; // next の距離も更新
            }
        }
    }
    return dist;
}

// from のグリッド上の ori を右端か左端に持っていく
inline void mapTranslation(VVI &from, VVI &to, P ori, char c) {
    int height = from.size();
    int width = from[0].size();
    int diffX = 0;
    if (c == 'R') { // ori を右端に持っていく
        for (int j = ori.first + 1; j < width; j++)
            REP(i, height)
                to.at(i).at(j - (ori.first + 1)) = from.at(i).at(j);
        for (int j = 0; j < ori.first + 1; j++)
            REP(i, height)
                to.at(i).at(j + (width - ori.first - 1)) = from.at(i).at(j);
    }
    else if (c == 'L') { // ori を左端に持っていく
        for (int j = ori.first; j < width; j++)
            REP(i, height)
                to.at(i).at(j - ori.first) = from.at(i).at(j);
        for (int j = 0; j < ori.first; j++)
            REP(i, height)
                to.at(i).at(j + (width - ori.first)) = from.at(i).at(j);
    }
}

// from 上の両端にある ori を元の位置に戻す
inline void mapUndoTranslation(VVI &from, VVI &to, P ori, char c) {
    int height = from.size();
    int width = from[0].size();
    // 右端を ori まで戻す
    if (c == 'R') {
        for (int j = 0; j < width - ori.first - 1; j++)
            REP(i, height)
                to.at(i).at(j + ori.first + 1) = from.at(i).at(j);
        for (int j = width  - ori.first - 1; j < width; j++)
            REP(i, height)
                to.at(i).at(j - width + ori.first + 1) = from.at(i).at(j);
    }
    // 左端を ori まで戻す
    if (c == 'L') {
        for (int j = 0; j < width - ori.first; j++)
            REP(i, height)
                to.at(i).at(j + ori.first) = from.at(i).at(j);
        for (int j = width - ori.first; j < width; j++)
            REP(i, height)
                to.at(i).at(j - width + ori.first) = from.at(i).at(j);
    }
}

// from のマップ上の ori を from の中心に持っていく
inline void mapTranslation2Center(VVI &from, VVI &to, P ori) {
    int height = from.size();
    int width = from[0].size();
    int diffX = diffX =  ori.first - (width / 2);
    // ori を中心に持っていく
    if (diffX <= 0) {
        for (int j = ori.first; j < width + diffX; j++)
            REP(i, height)
                to.at(i).at(j - diffX) = from.at(i).at(j);
        for (int j = 0; j < ori.first; j++)
            REP(i, height)
                to.at(i).at(j - diffX) = from.at(i).at(j);
        for (int j = width + diffX; j < width; j++)
            REP(i, height)
                to.at(i).at(j - width - diffX) = from.at(i).at(j);
    } else {
        for (int j = ori.first; j < width; j++)
            REP(i, height)
                to.at(i).at(j - diffX) = from.at(i).at(j);
        for (int j = diffX; j < ori.first; j++)
            REP(i, height)
                to.at(i).at(j - diffX) = from.at(i).at(j);
        for (int j = 0; j < diffX; j++)
            REP(i, height)
                to.at(i).at(width - diffX + j) = from.at(i).at(j);
    }
}

inline int calShortestDist(P from, P to, VVI map) {
    int height = map.size();
    int width = map[0].size();
    int diffX =  from.first - (width / 2);
    VVI dist(height, VI(width, 999)); // 各セルの最短距離 (訪れていないところは 999 としておく)

    bool loop = false;
    REP(i, height)
        if (map[i][0] == 1)
            loop = true;

    if (loop) {
        VVI fieldRight(height, VI(width, 0)); // from が右端にある field
        mapTranslation(map, fieldRight, from, 'R'); // from を右端に持っていく
        P fromRight = make_pair(width - 1, from.second);
        VVI distRight = calDist(fromRight, to, fieldRight); // from を右端に持っていったときの最短経路図
        VVI tmpRight(height, VI(width, 999));// from を元に戻したときの最短経路図
        mapUndoTranslation(distRight, tmpRight, from, 'R');

        VVI fieldLeft(height, VI(width, 0)); // from が左端にある field
        mapTranslation(map, fieldLeft, from, 'L'); // from を左端に持っていく
        P fromLeft = make_pair(0, from.second);
        VVI distLeft = calDist(fromLeft, to, fieldLeft); // from を左端に持っていったときの最短経路図
        VVI tmpLeft(height, VI(width, 999)); // from を元に戻したときの最短経路図
        mapUndoTranslation(distLeft, tmpLeft, from, 'L');

        VVI fieldCenter(height, VI(width, 0)); // from が中央にある field
        mapTranslation2Center(map, fieldCenter, from); // from を中央に持っていく
        P fromCenter = make_pair(from.first - diffX, from.second);
        VVI distCenter = calDist(fromCenter, to, fieldCenter); // from を中央に持っていったときの最短経路図
        VVI tmpCenter(height, VI(width, 999)); // from を元に戻したときの最短経路図
        P inverseCenter = make_pair(width - 1 - from.first, from.second);
        mapTranslation2Center(distCenter, tmpCenter, inverseCenter);

        REP(i, height) REP(j, width)
            dist.at(i).at(j) = min({tmpRight.at(i).at(j), tmpLeft.at(i).at(j), tmpCenter.at(i).at(j)});
        // cerr << "dist: " << height << ' ' << width << endl;
        // cerr << "ori: " << "y: " << from.second << " x: " << from.first << endl;
        // REP(i, height) {
        //     REP(j, width) {
        //         if (dist[i][j] == 999)
        //             dist[i][j] = -1;
        //         cerr << setw(2) << dist[i][j];
        //     }
        //     cerr << endl;
        // }
    } else {
        dist = calDist(from, to, map);
        // REP(i, height) {
        //     REP(j, width) {
        //         if (dist[i][j] == 999)
        //             dist[i][j] = -1;
        //         cerr << setw(2) << dist[i][j];
        //     }
        //     cerr << endl;
        // }
    }
    return dist[to.second][to.first];
}

/**
 * Grab the pellets as fast as you can!
 **/

int main() {
    chrono::system_clock::time_point start, end;
    int width, height; // top left corner is (x=0, y=0)
    cin >> width >> height; cin.ignore();

    VVI map(height, VI(width, 0));
    REP(i, height) {
        string row;
        getline(cin, row); // one line of the grid: space " " is floor, pound "#" is wall
        REP(j, width) {
            if (row[j] == ' ') map[i][j] = 1;
            else if (row[j] == '#') map[i][j] = 0;
        }
    }

    VVI mapCopy(height, VI(width, 0));
    REP(i, height) REP(j, width)
        mapCopy[i][j] = map[i][j];

    // 床の枚数を数える
    int countFloor = 0;
    REP(i, height) REP(j, width) {
        countFloor += map[i][j];
    }

    VP myPac;
    VP opponentPac;
    std::map<P, int> pellet;
    double time = static_cast<double>(chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0);
    cerr << "time: " << time << endl;
    // game loop
    while (1) {
        REP(i, height) REP(j, width)
            map[i][j] = mapCopy[i][j];

        int myScore;
        int opponentScore;
        cin >> myScore >> opponentScore; cin.ignore();

        int visiblePacCount; // all your pacs and enemy pacs in sight
        cin >> visiblePacCount; cin.ignore();
        myPac.clear();
        opponentPac.clear();
        REP(i, visiblePacCount) {
            int pacId; // pac number (unique within a team)
            bool mine; // true if this pac is yours
            int x, y; // position in the grid
            string typeId; // unused in wood leagues
            int speedTurnsLeft; // unused in wood leagues
            int abilityCooldown; // unused in wood leagues
            cin >> pacId >> mine >> x >> y >> typeId >> speedTurnsLeft >> abilityCooldown; cin.ignore();
            if (mine) myPac.push_back(make_pair(x, y));
            else opponentPac.push_back(make_pair(x, y));
            map[y][x] = 0;
        }

        int visiblePelletCount; // all pellets in sight
        cin >> visiblePelletCount; cin.ignore();
        pellet.clear();
        REP(i, visiblePelletCount) {
            int x, y, v; // v is amount of points this pellet is worth
            cin >> x >> y >> v; cin.ignore();
            pellet.insert(make_pair(make_pair(x, y), v));
        }

        start = chrono::system_clock::now();
        VP pellet10;
        VP pellet1;
        for (auto itr = pellet.begin(); itr != pellet.end(); itr++) {
            if (itr->second == 10)
                pellet10.push_back(itr->first);
            if (itr->second == 1)
                pellet1.push_back(itr->first);
        }

        std::map<int, P> target;
        for (auto itr = myPac.begin(); itr != myPac.end(); itr++) {
            int tmp = 1000;
            if (pellet10.size()) {
                for (auto itr2 = pellet10.begin(); itr2 != pellet10.end(); itr2++) {
                    int dist = calShortestDist(*itr, *itr2, map);
                    if (dist < tmp) {
                        tmp = dist;
                        target[distance(myPac.begin(), itr)] = *itr2;
                    }
                }
            } else {
                for (auto itr2 = pellet1.begin(); itr2 != pellet1.end(); itr2++) {
                    int dist = calShortestDist(*itr, *itr2, map);
                    if (dist < tmp) {
                        tmp = dist;
                        target[distance(myPac.begin(), itr)] = *itr2;
                    }
                }
                map[target[distance(myPac.begin(), itr)].second][target[distance(myPac.begin(), itr)].first] = 0;
            }
        }
        // for (auto itr = target.begin(); itr != target.end(); itr++)
        //     cerr << itr->first << " x: " << itr->second.first << " y: " << itr->second.second << endl;
        end = chrono::system_clock::now();

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        REP(i, myPac.size()) {
            cout << "MOVE" << ' ' << i << ' ' << target[i].first << ' ' << target[i].second; // MOVE <pacId> <x> <y>
            if (i == myPac.size() - 1) cout << endl;
            else cout << "|";
        }
    }
}
