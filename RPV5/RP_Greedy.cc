// RP_Greedy.cc 

#include "RP_Greedy.hh"
#include "Random.hh"
#include <algorithm>
#include <numeric>

using namespace std;


unsigned ManhattanDistance (point p1, point p2)
{
    return (abs(int(p1.row - p2.row)) + abs(int(p1.col - p2.col)));
}


unsigned PlaceRandomizedRouter(const RP_Input& in, RP_Output& out)
{
    unsigned current_coverage;
    unsigned attempts = 0;
    unsigned max_coverage = 0;
    const unsigned MAX_ATTEMPTS = 1000;
    point selected, max_coverage_found;

    while (attempts < MAX_ATTEMPTS)
    {
        do
        {
            selected.row = Random(0, in.Rows()-1);
            selected.col = Random(0, in.Columns()-1);
        } while (!in.IsDot(selected.row, selected.col) || (in.RouterPrice() + (ManhattanDistance(selected, ClosestBackbonePoint(out, in, selected)) * in.BackbonePrice()) >= out.RemainingBudget()));
        
        if ((current_coverage = out.CellDegree(selected).size()) > max_coverage)
        {
            max_coverage_found.row = selected.row;
            max_coverage_found.col = selected.col;
            max_coverage = current_coverage;
        }
        attempts++;
    }

    out.InsertRouter(max_coverage_found.row, max_coverage_found.col);
    cout << "Covered: " << out.TotalCoveredPoints() << endl;

    return max_coverage;
}


void ConnectRouterWithBackbone(RP_Output& out, const RP_Input& in, point router)
{
    point temp, selected_bb_cell;
    unsigned new_distance;

    // select the backbone point
    selected_bb_cell = ClosestBackbonePoint(out, in, router);
    // cout << "selectedbbcell: " << selected_bb_cell.row << " " << selected_bb_cell.col << endl;

    unsigned current_distance = ManhattanDistance(selected_bb_cell, router);

    while (current_distance > 0)
    {
        temp = MakeRandomMove(selected_bb_cell);
        new_distance = ManhattanDistance(temp, router);
        if (new_distance < current_distance)
        {
            current_distance = new_distance;
            out.InsertBackboneCell(temp.row, temp.col);
            selected_bb_cell = temp;
        }
    }
}


// move from p to one point of his neighborhood
point MakeRandomMove(point p) 
{  
    point p2;

    unsigned r_diff = Random(-1, 1);
    unsigned c_diff = Random (-1, 1);
    p2.row = p.row + r_diff;
    p2.col = p.col + c_diff;

    return p2;
}


point ClosestBackbonePoint (const RP_Output& out, const RP_Input& in, point router)
{
    unsigned minimum_distance = ManhattanDistance(in.StartBackbone(), router);
    point closest = in.StartBackbone();
    unsigned current_distance;
    

    for (unsigned i = 0; i < out.BackboneSize(); i++)
    {
        current_distance = ManhattanDistance(out.BackboneCell(i), router);
        if (current_distance < minimum_distance)
        {
            closest = out.BackboneCell(i);
            minimum_distance = current_distance;
        }
    }
    
    return closest;
}


void GreedyRP(const RP_Input& in, RP_Output& out)
{ 
    unsigned new_coverage;
    point router;

    do
    {    
        if (out.RemainingBudget() >= int(in.RouterPrice()))
        {
            new_coverage = PlaceRandomizedRouter(in, out);
            // cout << "Remaining Budget: "  << out.RemainingBudget() << endl;

            ConnectRouterWithBackbone(out, in, router);
            // cout << "New Backbone:" << endl;
            // PrintVec(out.Backbone());
            // // cout << "Remaining Budget: "  << out.RemainingBudget() << endl;
        }
        else
            new_coverage = 0;
    } while(new_coverage > 0);
    // cout << "Covered: " << endl;
    // PrintVec(out.Covered());
    cout << out;
    cout << "Final Score: " << out.ComputeScore() << endl;
    cout << "Remaining budget: " << out.RemainingBudget() << endl;
}


bool PointIsInVec (point pt, vector<point> vec) 
{
    for (unsigned i = 0; i < vec.size(); i++)
        if (pt.row == vec[i].row && pt.col == vec[i].col)
            return 1;

    return 0;
}


void PrintVec (vector<point> vec) 
{
    for (unsigned i = 0; i < vec.size(); i++)
    {
        cout << vec[i].row << " " << vec[i].col;
        if (i != vec.size()-1)
            cout << " -> ";
    }
        
    cout << endl;
}
