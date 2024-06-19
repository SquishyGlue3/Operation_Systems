#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <deque>

using namespace std;

class AbstractList
{
public:
    virtual void push_front(int v) = 0;
    virtual int front() = 0;
    virtual void pop_front() = 0;
    virtual bool empty() = 0;
    virtual ~AbstractList() {}
};

class myList : public AbstractList
{
private:
    list<int> mylist;

public:
    void push_front(int v) override
    {
        mylist.push_front(v);
    }
    int front() override
    {
        return mylist.front();
    }
    void pop_front() override
    {
        mylist.pop_front();
    }
    bool empty() override
    {
        return mylist.empty();
    }
};

class myDeque : public AbstractList
{
private:
    deque<int> mydeque;

public:
    void push_front(int v) override
    {
        mydeque.push_front(v);
    }
    int front() override
    {
        return mydeque.front();
    }
    void pop_front() override
    {
        mydeque.pop_front();
    }
    bool empty() override
    {
        return mydeque.empty();
    }
};

class Graph
{
protected:
    vector<vector<int>> edges;
    vector<int> vertices;

public:
    Graph(vector<int> vertices, vector<vector<int>> edges) : vertices(vertices), edges(edges) {}
    virtual ~Graph() {}
    virtual Graph *transpose() = 0;
    virtual void findSCCs(const string &listType) = 0;

    vector<int> getVertices() const
    {
        return vertices;
    }
    vector<vector<int>> getEdges() const
    {
        return edges;
    }
    int verticesNum() const
    {
        return vertices.size();
    }
    int edgesNum() const
    {
        return edges.size();
    }
};

class GraphAdj : public Graph
{
private:
    vector<vector<int>> adj;
    vector<vector<int>> revAdj;

public:
    GraphAdj(vector<int> vertices, vector<vector<int>> edges) : Graph(vertices, edges)
    {
        adj.resize(vertices.size());
        revAdj.resize(vertices.size());
        for (int i = 0; i < edges.size(); i++)
        {
            adj[edges[i][0] - 1].push_back(edges[i][1]);    // -1 because the vertices start from 1
            revAdj[edges[i][1] - 1].push_back(edges[i][0]); // -1 because the vertices start from 1
        }
    }

    void DFS(int v, vector<bool> &visited, AbstractList *list)
    {
        // Add this check
        if (v <= 0 || v > adj.size())
        {
            return;
        }

        visited[v - 1] = true;
        for (int i = 0; i < adj[v - 1].size(); i++)
        {
            if (!visited[adj[v - 1][i] - 1])
            {
                DFS(adj[v - 1][i], visited, list);
            }
        }
        list->push_front(v);
    }

    void findSCCs(const string &listType) override
    {
        AbstractList *list;
        if (listType == "deque")
        {
            list = new myDeque();
        }
        else if (listType == "list")
        {
            list = new myList();
        }
        else
        {
            cout << "Invalid list type" << endl;
            return;
        }

        vector<bool> visited(verticesNum(), false);
        for (int i = 1; i < verticesNum(); i++)
        {
            if (!visited[i-1])
            {
                DFS(i, visited, list);
            }
        }

        GraphAdj *gT = transpose();
        visited.assign(verticesNum(), false);
        while (!list->empty())
        {
            int v = list->front();
            list->pop_front();
            if (!visited[v])
            {
                AbstractList *list2;
                if (listType == "deque")
                {
                    list2 = new myDeque();
                }
                else if (listType == "list")
                {
                    list2 = new myList();
                }
                else
                {
                    cout << "Invalid list type" << endl;
                    return;
                }
                gT->DFS(v, visited, list2);
                cout << "{ ";
                while (!list2->empty())
                {
                    cout << list2->front() << " ";
                    list2->pop_front();
                }
                cout << "}" << endl;
            }
        }
    }

    GraphAdj *transpose() override
    {
        return new GraphAdj(vertices, revAdj);
    }
};

class GraphChildren : public Graph
{
private:
    vector<vector<int>> children;

public:
    GraphChildren(vector<int> vertices, vector<vector<int>> edges) : Graph(vertices, edges)
    {
        children.resize(vertices.size() + 1);
        for (const auto &edge : edges)
        {
            children[edge[0]].push_back(edge[1]);
        }
    }

    GraphChildren *transpose() override
    {
        vector<vector<int>> revEdges;
        for (const auto &edge : edges)
        {
            revEdges.push_back({edge[1], edge[0]});
        }
        return new GraphChildren(this->vertices, revEdges);
    }

    void DFS(int v, vector<bool> &visited, AbstractList *list)
    {
        visited[v] = true;
        for (const auto &u : children[v])
        {
            if (!visited[u])
            {
                DFS(u, visited, list);
            }
        }
        list->push_front(v);
    }

    void findSCCs(const string &listType) override
    {
        AbstractList *list;
        if (listType == "deque")
        {
            list = new myDeque();
        }
        else if (listType == "list")
        {
            list = new myList();
        }
        else
        {
            cout << "Invalid list type" << endl;
            return;
        }

        vector<bool> visited(verticesNum(), false);
        for (int i = 0; i < verticesNum(); i++)
        {
            if (!visited[i])
            {
                DFS(i, visited, list);
            }
        }

        GraphChildren *gT = transpose();
        visited.assign(verticesNum(), false);
        while (!list->empty())
        {
            int v = list->front();
            list->pop_front();
            if (!visited[v])
            {
                AbstractList *list2;
                if (listType == "deque")
                {
                    list2 = new myDeque();
                }
                else if (listType == "list")
                {
                    list2 = new myList();
                }
                else
                {
                    cout << "Invalid list type" << endl;
                    return;
                }
                gT->DFS(v, visited, list2);
                cout << "{ ";
                while (!list2->empty())
                {
                    cout << list2->front() << " ";
                    list2->pop_front();
                }
                cout << "}" << endl;
            }
        }
    }
};

int main()
{
    vector<vector<int>> m;
    vector<int> n;
    bool aflag = 0, bflag = 0;
    int vertices, edges;
    cout << "Please insert 2 numbers, first will be number of vertices and second will be number of edges" << endl;
    cin >> vertices >> edges;
    cout << "Please insert " << edges << " pairs of numbers, each pair represents an edge" << endl;
    for (int i = 0; i < edges; i++)
    {
        int a, b;
        cout << "Please insert pair number " << i + 1 << endl;
        cin >> a >> b;
        aflag = 0;
        bflag = 0;
        for (size_t j = 0; j < n.size(); j++)
        {
            if (n[j] == a)
            {
                aflag = 1;
                break;
            }
        }
        for (size_t j = 0; j < n.size(); j++)
        {
            if (n[j] == b)
            {
                bflag = 1;
                break;
            }
        }
        if (aflag == 0)
        {
            n.push_back(a); // if a is not in the vertices list, add it
        }
        if (bflag == 0)
        {
            n.push_back(b); // if b is not in the vertices list, add it
        }
        vector<int> edge = {a, b};
        m.push_back(edge); // add the edge to the list of edges
    }
    string graphType;
    cout << "Please insert the type of graph you want to use, 'adj' or 'children'" << endl;
    cin >> graphType;

    Graph *g;
    if (graphType == "adj")
    {
        g = new GraphAdj(n, m);
    }
    else if (graphType == "children")
    {
        g = new GraphChildren(n, m);
    }
    else
    {
        cout << "Invalid graph type" << endl;
        return 0;
    }

    cout << "Strongly Connected Components in the graph are: \n";
    string listType;
    cout << "Please insert the type of list you want to use, 'deque' or 'list'" << endl;
    cin >> listType;
    g->findSCCs(listType);
    return 0;
}