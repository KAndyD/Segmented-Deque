#include <iostream>
#include <vector>

std::vector <bool> Visited; // Вектор посещенных вершин
std::vector <int> Path; // Вектор пути, по которому мы идем

// На ввод текущая вершина, матрица смежности, количество вершин
int Hamilton(int curr, const std::vector<std::vector<int>>& M, int n)
{
    Path.push_back(curr); // В путь добавляем текущую вершину

    // Проверка, нашли ли мы Гамильтонов цикл
    if(Path.size() == n){

        // Проверка на то, есть ли между последней и первой вершиной пути связь, чтобы был цикл
        if(M[Path.back()][Path[0]] == 1){
            return 1; 

        } else {
            // Если нет - удаляем последнюю вершину из пути
            Path.pop_back();
            return 0;
        }
    }

    // Помечаем текущую вершину, как посещенную
    Visited[curr] = 1;

    // От текущей вершины проходимся рекурсивно по всем остальным
    for(int nxt = 0; nxt < n; ++nxt){

        // Если между текущей и следующей вершиной вообще есть связь, а также следующая вершина не была посещена
        if(M[curr][nxt] == 1 && !Visited[nxt]){
            // Рекурсивный вызов
            if(Hamilton(nxt, M, n)){
                return true;
            }
        }
    }

    // Если из текущей вершины мы все же не нашли Гамильтонов цикл - то помечаем ее как не посещенную и удаляем из пути
    Visited[curr] = 0;
    Path.pop_back();
    return 0;
}


void findHamiltonianCycle(const std::vector<std::vector<int>>& graph) {
    int n = graph.size();
    Visited.assign(n, false);
    Path.clear();
    
    // Начинаем с вершины 0 (можно выбрать любую)
    if (Hamilton(0, graph, n)) {
        std::cout << "Гамильтонов цикл найден: ";
        for (int v : Path) {
            std::cout << v << " ";
        }
        // Выводим первую вершину снова, чтобы показать цикл
        std::cout << Path[0] << std::endl;
    } else {
        std::cout << "Гамильтонов цикл не найден" << std::endl;
    }
}

int main() {
    // Пример графа с Гамильтоновым циклом
    std::vector<std::vector<int>> graph1 = {
        {0, 1, 0, 0, 1},
        {1, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 1},
        {1, 0, 0, 1, 0}
    };
    
    std::cout << "Граф 1 (с циклом):" << std::endl;
    findHamiltonianCycle(graph1);
    
    // Пример графа без Гамильтонова цикла
    std::vector<std::vector<int>> graph2 = {
        {0, 1, 0, 0},
        {1, 0, 1, 0},
        {0, 1, 0, 1},
        {0, 0, 1, 0}
    };
    
    std::cout << "\nГраф 2 (без цикла):" << std::endl;
    findHamiltonianCycle(graph2);
    
    return 0;
}