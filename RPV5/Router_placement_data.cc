//  Router_placement_data.cpp

#include "Router_placement_data.hh"


unsigned FindRouterCoverageArea (const RP_Input& in, point router) 
{
    unsigned step = 1;

    do
    {
        for (int r=router.row-step; r <= router.row+step; r++)
        {
            for (int c=router.col-step; c <= router.col+step; c++)
            {
                if (r >= 0 && r < in.Rows() && c >= 0 && c < in.Columns())
                {
                    if (in.IsWall(r,c))
                        return step-1;
                }
                else
                    return step-1;
            }
        }
        step++;
    } while (step <= in.Radius());

    return in.Radius();
}


RP_Input::RP_Input(string file_name)
{  
    ifstream is(file_name);
    if (!is) 
    {
        cerr << "Cannot open file " <<  file_name << endl;
	    exit(1);
    }

    is >> rows >> columns >> radius
       >> b_price >> r_price >> budget
       >> start_backbone.row >> start_backbone.col;

    cells.resize(rows, vector<char>(columns));

    for (unsigned r = 0; r < rows; r++)
    {
        for (unsigned c = 0; c < columns; c++)
        {
            char ch;
            is >> ch;
            cells[r][c] = ch;
        }
    }
}


ostream& operator<<(ostream& os, const RP_Input& in)
{
    for (unsigned r = 0; r < in.rows; r++)
    {
        for (unsigned c = 0; c < in.columns; c++)
        {
            os << in.cells[r][c];
        }
        cout << endl;
    }
    return os;
}


RP_Output::RP_Output(const RP_Input& my_in) : in(my_in) // ?????
{
    out_cells = in.Cells(); // for for
    backbone.push_back(in.StartBackbone()); 
    out_cells[in.StartBackbone().row][in.StartBackbone().col] = 'b';
    remaining_budget = in.Budget()-in.BackbonePrice();
    total_covered_points = 0;
}


void RP_Output::InsertRouter(unsigned r, unsigned c)
{
    point router;
    vector<point> router_coverage;

    if (!in.IsWall(r, c))
    {
        // update cell
        out_cells[r][c] = 'r';

        router.row = r;
        router.col = c;
    
        routers.push_back(router);
        cout << "Placed router in cell " << router.row << " " << router.col << endl;

        // update coverage
        router_coverage = CellDegree(router); 
        cout << "Il router copre " << router_coverage.size() << " celle." << endl;
        total_covered_points += router_coverage.size();
        for (unsigned i = 0; i < router_coverage.size(); i++)
            if (out_cells[router_coverage[i].row][router_coverage[i].col] != 'r')
                out_cells[router_coverage[i].row][router_coverage[i].col] = '1';
            
        // update budget
        remaining_budget -= in.RouterPrice();
    }
}


void RP_Output::InsertBackboneCell(unsigned r, unsigned c)
{
    point bb_cell;
    bb_cell.row = r;
    bb_cell.col = c;

    if (!IsInBackbone(bb_cell))
        backbone.push_back(bb_cell);

    remaining_budget -= in.BackbonePrice();
}


// dato un punto, restituisce che celle coprirebbe se fosse un router.
vector<point> RP_Output::CellDegree(point pt)
{
    point temp;
    unsigned new_radius;
    vector<point> cell_degree;
        
    new_radius = FindRouterCoverageArea(in, pt);

    for (int r=pt.row-new_radius; r <= pt.row+new_radius; r++)
    {
        for (int c=pt.col-new_radius; c <= pt.col+new_radius; c++)
        {
            if (out_cells[r][c] == '.')
            {
                temp.row = r;
                temp.col = c;
                cell_degree.push_back(temp);
            }
        }
    }
    return cell_degree;
}


bool RP_Output::IsCovered(point pt) 
{
    vector<point> router_coverage;

    for (unsigned i = 0; i < RoutersSize(); i++)
    {
        router_coverage = CellDegree(routers[i]);
        for (unsigned j = 0; j < router_coverage.size(); j++)
        {
            if (pt.row == router_coverage[j].row && pt.col == router_coverage[j].col)
            { 
                return 1;
            }
        }
    }
    return 0;
}


// void RP_Output::RemoveRouter(point router)
// {
//     out_cells[router.row][router.col] = in.cells[router.row][router.col];
//     routers.delete()
//     out.remaining_budget += in.RouterPrice();
// }

 
// void RP_Output::RemoveBackboneCell (point bb_cell)
// {
//     backbone.delete()
//     out.remaining_budget += in.BackbonePrice();
// }


ostream& operator<<(ostream& os, const RP_Output& out)
{
    for (unsigned r = 0; r < out.Rows(); r++)
    {
        for (unsigned c = 0; c < out.Columns(); c++)
        {
            os << out.out_cells[r][c];
        }
        cout << endl;
    }
    return os;
}


bool RP_Output::IsInBackbone (point pt) const
{
    for (unsigned i = 0; i < backbone.size(); i++)
        if (pt.row == backbone[i].row && pt.col == backbone[i].col)
            return 1;

    return 0;
}



