#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

const int MAP_SIZE = 20;
const int OBSTACLE = 5;
const int PATH = 6;

struct Node {
    double g, h, f;
    int x, y;
    Node* parent;

    Node(int x_pos, int y_pos, Node* ptr_parent = nullptr) {
        x = x_pos;
        y = y_pos;
        g = 0.0;
        h = 0.0;
        f = 0.0;
        parent = ptr_parent;
    }
};

double heurystyka(int x_current, int y_current, int x_finish, int y_finish) {
    return sqrt(pow(x_current - x_finish, 2) + pow(y_current - y_finish, 2));
}

bool mapreader(int grid[MAP_SIZE][MAP_SIZE]) {
    ifstream file("grid.txt");
    if (!file.is_open()) {
        cerr << "nie ma mapy\n\n";
        return false;
    }

    for (int i = MAP_SIZE-1; i >= 0; i--) {
        for (int j = 0; j < MAP_SIZE; j++) {
            file >> grid[i][j];
        }
    }
    return true;
}

void printMap(int grid[MAP_SIZE][MAP_SIZE]) {
    for (int i = MAP_SIZE - 1; i >= 0; i--) {
        for (int j = 0; j < MAP_SIZE; j++) {
            cout << grid[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

bool czyNaLiscieZamknietej(vector<Node*> listazam, int szukany_x, int szukany_y) {
    for (Node* wezel : listazam) {
        if (wezel->x == szukany_x && wezel->y == szukany_y) {
            return true;
        }
    }
    return false;
}

Node* szukanyNaLiscieOtwartej(vector<Node*> listaotw, int szukany_x, int szukany_y) {
    for (Node* wezel : listaotw) {
        if (wezel->x == szukany_x && wezel->y == szukany_y) {
            return wezel;
        }
    }
    return nullptr;
}

Node* astar(int grid[MAP_SIZE][MAP_SIZE], int x_pocz, int y_pocz, int x_kon, int y_kon) {
    if ((x_pocz < 0 || x_pocz > MAP_SIZE-1 || y_pocz < 0 || y_pocz > MAP_SIZE-1) &&
        (x_kon < 0 || x_kon > MAP_SIZE-1 || y_kon < 0 || y_kon > MAP_SIZE-1)) {
        cout << "Cel i poczatek znajdujom siem poza granicami mapy gg\n\n";
        return nullptr;
    }

    if (x_pocz < 0 || x_pocz > MAP_SIZE-1 || y_pocz < 0 || y_pocz > MAP_SIZE-1) {
        cout << "Poczatek jest poza granicami mapy gg\n\n";
        return nullptr;
    }

    if (x_kon < 0 || x_kon > MAP_SIZE-1 || y_kon < 0 || y_kon > MAP_SIZE-1) {
        cout << "Cel jest poza granicami mapy gg\n\n";
        return nullptr;
    }

    if (grid[y_kon][x_kon] == OBSTACLE && grid[y_pocz][x_pocz] == OBSTACLE) {
        cout << "Cel i poczatek to przeszkody gg\n\n";
        return nullptr;
    }

    if (grid[y_kon][x_kon] == OBSTACLE) {
        cout << "Cel to przeszkoda gg\n\n";
        return nullptr;
    }

    if (grid[y_pocz][x_pocz] == OBSTACLE) {
        cout << "Poczatek to przeszkoda gg\n\n";
        return nullptr;
    }

    vector<Node*> lotwarta;
    vector<Node*> lzamknieta;
    Node* starter= new Node(x_pocz,y_pocz);

    if (x_pocz == x_kon && y_pocz == y_kon) {
        return starter;
    }

    starter->h = heurystyka(x_pocz, y_pocz, x_kon, y_kon);
    starter->f = starter->g + starter->h;
    lzamknieta.push_back(starter);

    int dir_x[4] = {0,0,-1,1};
    int dir_y[4] = {1,-1,0,0};

    for (int i = 0; i < 4; i++) {
        int next_x = starter->x + dir_x[i];
        int next_y = starter->y + dir_y[i];

        if (next_x < 0 || next_x > MAP_SIZE-1 || next_y < 0 || next_y > MAP_SIZE-1) {
            continue;
        }

        if (grid[next_y][next_x] == OBSTACLE) {
            continue;
        }

        if (czyNaLiscieZamknietej(lzamknieta, next_x, next_y)) {
            continue;
        }

        Node* somsiad = new Node(next_x, next_y);
        somsiad->parent = starter;
        somsiad->g = starter->g +1;
        somsiad->h = heurystyka(next_x, next_y, x_kon, y_kon);
        somsiad->f = somsiad->g + somsiad->h;

        lotwarta.push_back(somsiad);
    }

    while (!lotwarta.empty()){
        int best_idx = 0;
        for (int i = 0; i < lotwarta.size(); i++) {
            if (lotwarta[i]->f <= lotwarta[best_idx]->f) {
                best_idx = i;
            }
        }
        Node* current = lotwarta[best_idx];

        lotwarta.erase(lotwarta.begin() + best_idx);
        lzamknieta.push_back(current);

        if (current->x == x_kon && current->y == y_kon) {
            return current;
        }

        int kier_x[4] = {0,0,-1,1};
        int kier_y[4] = {1,-1,0,0};

        for (int i = 0; i < 4; i++) {
            int new_x = current->x + kier_x[i];
            int new_y = current->y + kier_y[i];

            if (new_x < 0 || new_x > MAP_SIZE-1 || new_y < 0 || new_y > MAP_SIZE-1) {
                continue;
            }

            if (grid[new_y][new_x] == OBSTACLE) {
                continue;
            }

            if (czyNaLiscieZamknietej(lzamknieta, new_x, new_y)) {
                continue;
            }

            Node* somsiad_somsiadow = new Node(new_x, new_y);
            somsiad_somsiadow->parent = current;
            somsiad_somsiadow->g = current->g +1;
            somsiad_somsiadow->h = heurystyka(new_x, new_y, x_kon, y_kon);
            somsiad_somsiadow->f = somsiad_somsiadow->g + somsiad_somsiadow->h;

            Node* wezel_na_otwartej = szukanyNaLiscieOtwartej(lotwarta, new_x, new_y);

            if (wezel_na_otwartej == nullptr) {
                lotwarta.push_back(somsiad_somsiadow);
            }

            else {
                if (somsiad_somsiadow->g < wezel_na_otwartej->g) {
                    wezel_na_otwartej->g = somsiad_somsiadow->g;
                    wezel_na_otwartej->f = somsiad_somsiadow->f;
                    wezel_na_otwartej->parent = current;
                }
                delete somsiad_somsiadow;
            }
        }
    }
    return nullptr;
}

void pathfinder(int grid[MAP_SIZE][MAP_SIZE], Node* finisz) {
    Node* temp = finisz;
    while (temp != nullptr) {
        grid[temp->y][temp->x] = PATH;
        temp = temp->parent;
    }
}

int main() {
    int map[MAP_SIZE][MAP_SIZE];
    bool czyWczytano = mapreader(map);

    if (czyWczytano == true) {
        printMap(map);
        Node* result = astar(map, 0, 0, 19, 19);

        if (result!=nullptr) {
            cout << "Jest trasa, nice!\n\n";
            pathfinder(map, result);
            printMap(map);
        }

        else {
            cout << "Nie ma mozliwosci dotarcia do celu womp womp :(\n\n";
        }
    }


    return 0;
}
