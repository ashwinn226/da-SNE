/*
 *
 * Copyright (c) 2014, Laurens van der Maaten (Delft University of Technology)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Delft University of Technology.
 * 4. Neither the
 name of the Delft University of Technology nor the names of
 *    its contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY LAURENS VAN DER MAATEN ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL LAURENS VAN DER MAATEN BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */


#ifndef DA_SPTREE_H
#define DA_SPTREE_H

using namespace std;

/*
class Cell {

    unsigned int dimension;
    double* corner;
    double* width;
    
    
public:
    Cell(unsigned int inp_dimension);
    Cell(unsigned int inp_dimension, double* inp_corner, double* inp_width);
    ~Cell();
    
    double getCorner(unsigned int d);
    double getWidth(unsigned int d);
    void setCorner(unsigned int d, double val);
    void setWidth(unsigned int d, double val);
    bool containsPoint(double point[]);
};
*/

class DA_SPTree : public Abs_SPTree
{
  /*
    // Fixed constants
    static const unsigned int QT_NODE_CAPACITY = 1;

    // A buffer we use when doing force computations
    double* buff;
    
    // Properties of this node in the tree
    DA_SPTree* parent;
    unsigned int dimension;
    bool is_leaf;
    unsigned int size;
    unsigned int cum_size;
  */
    double beta_ratio; 
    double log_beta_com; 

    // min_beta and max_beta, like cum_size are properties of *this particular node*
    double min_beta;
    double max_beta;

    // overall_beta_min is the smallest beta taken *over all the betas* 
    double overall_beta_min; 
    
    // Axis-aligned bounding box stored as a center with half-dimensions to represent the boundaries of this quad tree
    Cell* boundary;
    
    // Indices in this space-partitioning tree node, corresponding center-of-mass, and list of all children
    double* data;
    double* center_of_mass;
    unsigned int index[QT_NODE_CAPACITY];

    // Keep track of the betas here as well
    double* betas; 
    
    // Children
    DA_SPTree** children;
    unsigned int no_children;
    
public:
    DA_SPTree(unsigned int D, double* inp_data, double* inp_betas, double beta_min, unsigned int N);
    DA_SPTree(unsigned int D, double* inp_data, double* inp_betas, double beta_min, double* inp_corner, double* inp_width);
    DA_SPTree(unsigned int D, double* inp_data, double* inp_betas, double beta_min, unsigned int N, double* inp_corner, double* inp_width);
    DA_SPTree(DA_SPTree* inp_parent, unsigned int D, double* inp_data, double* inp_betas, double beta_min,
	      unsigned int N, double* inp_corner, double* inp_width);
    DA_SPTree(DA_SPTree* inp_parent, unsigned int D, double* inp_data, double* inp_betas, double beta_min,
	      double* inp_corner, double* inp_width);
    ~DA_SPTree();
    /*
    void setData(double* inp_data);
    DA_SPTree* getParent();
    void construct(Cell boundary);
    bool insert(unsigned int new_index);
    void subdivide();
    bool isCorrect();
    void rebuildTree();
    void getAllIndices(unsigned int* indices);
    unsigned int getDepth();
    */
    void computeNonEdgeForces(unsigned int point_index, double theta, double beta_thresh, double neg_f[], double* sum_Q,
			      int& total_count, double& total_time, double& emb_density);
    
    void computeNonEdgeForces(unsigned int point_index, double theta,
			      double neg_f[], double* sum_Q, int& total_count, double& total_time,
			      double& emb_density);
    void computeEdgeForces(unsigned int* row_P, unsigned int* col_P, double* val_P, int N, double* pos_f, bool lying);
    // void computeEdgeForces(unsigned int* row_P, unsigned int* col_P, double* val_P, int N, double* pos_f);
    // void print();
    
private:
    /*
    void init(DA_SPTree* inp_parent, unsigned int D, double* inp_data, double* inp_betas, double beta_min,
	      double* inp_corner, double* inp_width);
    void fill(unsigned int N);
    unsigned int getAllIndices(unsigned int* indices, unsigned int loc);
    bool isChild(unsigned int test_index, unsigned int start, unsigned int end);
    */
};

#endif
