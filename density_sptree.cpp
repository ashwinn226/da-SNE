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
 * 4. Neither the name of the Delft University of Technology nor the names of
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

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <ctime>
#include "cell.h"
#include "density_sptree.h"


// Default constructor for SPTree -- build tree, too!
SPTree::SPTree(unsigned int D, double* inp_data, double* emb_densities, double* log_emb_densities,
	       double* emb_densities_no_entropy, 
	       double* log_orig_densities, double* marg_Q, unsigned int N)
{
    
    // Compute mean, width, and height of current map (boundaries of SPTree)
    int nD = 0;
    double* mean_Y = (double*) calloc(D,  sizeof(double));
    double*  min_Y = (double*) malloc(D * sizeof(double)); for(unsigned int d = 0; d < D; d++)  min_Y[d] =  DBL_MAX;
    double*  max_Y = (double*) malloc(D * sizeof(double)); for(unsigned int d = 0; d < D; d++)  max_Y[d] = -DBL_MAX;
    for(unsigned int n = 0; n < N; n++) {
        for(unsigned int d = 0; d < D; d++) {
            mean_Y[d] += inp_data[n * D + d];
            if(inp_data[nD + d] < min_Y[d]) min_Y[d] = inp_data[nD + d];
            if(inp_data[nD + d] > max_Y[d]) max_Y[d] = inp_data[nD + d];
        }
        nD += D;
    }
    for(int d = 0; d < D; d++) mean_Y[d] /= (double) N;
    
    // Construct SPTree
    double* width = (double*) malloc(D * sizeof(double));
    for(int d = 0; d < D; d++) width[d] = fmax(max_Y[d] - mean_Y[d], mean_Y[d] - min_Y[d]) + 1e-5;
    init(NULL, D, inp_data, emb_densities, log_emb_densities, emb_densities_no_entropy,
	 log_orig_densities, marg_Q,mean_Y, width);
    fill(N);
    
    // Clean up memory
    free(mean_Y);
    free(max_Y);
    free(min_Y);
    free(width);
}


// Constructor for SPTree with particular size and parent -- build the tree, too!
SPTree::SPTree(unsigned int D, double* inp_data, double* emb_densities, double* log_emb_densities,
	       double* emb_densities_no_entropy, 
	   double* log_orig_densities, double* marg_Q, unsigned int N, double* inp_corner, double* inp_width)
{
  init(NULL, D, inp_data, emb_densities, log_emb_densities, emb_densities_no_entropy,
       log_orig_densities, marg_Q, inp_corner, inp_width);
    fill(N);
}


// Constructor for SPTree with particular size (do not fill the tree)
SPTree::SPTree(unsigned int D, double* inp_data, double* emb_densities, double* log_emb_densities,
	        double* emb_densities_no_entropy, 
	       double* log_orig_densities, double* marg_Q, double* inp_corner, double* inp_width)
{
  init(NULL, D, inp_data, emb_densities, log_emb_densities, emb_densities_no_entropy,
       log_orig_densities, marg_Q, inp_corner, inp_width);
}


// Constructor for SPTree with particular size and parent (do not fill tree)
SPTree::SPTree(SPTree* inp_parent, unsigned int D, double* inp_data, double* emb_densities, double* log_emb_densities,  double* emb_densities_no_entropy, 
	   double* log_orig_densities, double* marg_Q, double* inp_corner, double* inp_width) {
  init(inp_parent, D, inp_data, emb_densities, log_emb_densities, emb_densities_no_entropy,
       log_orig_densities, marg_Q,inp_corner, inp_width);
}


// Constructor for SPTree with particular size and parent -- build the tree, too!
SPTree::SPTree(SPTree* inp_parent, unsigned int D, double* inp_data, double* emb_densities, double* log_emb_densities,  double* emb_densities_no_entropy, 
	   double* log_orig_densities, double* marg_Q, unsigned int N, double* inp_corner, double* inp_width)
{
  init(inp_parent, D, inp_data, emb_densities, log_emb_densities,
       emb_densities_no_entropy, log_orig_densities, marg_Q, inp_corner, inp_width);
    fill(N);
}


// Main initialization function
void SPTree::init(SPTree* inp_parent, unsigned int D, double* inp_data, double* emb_densities, double* log_emb_densities, double* emb_densities_no_entropy, 
	   double* log_orig_densities, double* marg_Q, double* inp_corner, double* inp_width)
{
    parent = inp_parent;
    dimension = D;
    no_children = 2;
    for(unsigned int d = 1; d < D; d++) no_children *= 2;
    data = inp_data;
    all_emb_dens_no_entropy = emb_densities_no_entropy; 
    all_emb_dens = emb_densities; 
    all_log_emb_dens = log_emb_densities; 
    all_log_orig_dens = log_orig_densities; 
    all_marg_Q = marg_Q; 

    emb_density_com = 0.;
    log_emb_density_com = 0.; 
    log_orig_density_com = 0.; 
    marg_Q_com = 0.; 
    emb_density_no_entropy_com = 0.; 
    
    is_leaf = true;
    size = 0;
    cum_size = 0;
    
    boundary = new Cell(dimension);
    for(unsigned int d = 0; d < D; d++) boundary->setCorner(d, inp_corner[d]);
    for(unsigned int d = 0; d < D; d++) boundary->setWidth( d, inp_width[d]);
    
    children = (SPTree**) malloc(no_children * sizeof(SPTree*));
    for(unsigned int i = 0; i < no_children; i++) children[i] = NULL;

    center_of_mass = (double*) malloc(D * sizeof(double));
    for(unsigned int d = 0; d < D; d++) center_of_mass[d] = .0;
    
    buff = (double*) malloc(D * sizeof(double));
}


// Destructor for SPTree
SPTree::~SPTree()
{
    for(unsigned int i = 0; i < no_children; i++) {
        if(children[i] != NULL) delete children[i];
    }
    free(children);
    free(center_of_mass);
    free(buff);
    delete boundary;
}


// Update the data underlying this tree
void SPTree::setData(double* inp_data)
{
    data = inp_data;
}


// Get the parent of the current tree
SPTree* SPTree::getParent()
{
    return parent;
}


// Insert a point into the SPTree
bool SPTree::insert(unsigned int new_index)
{
    // Ignore objects which do not belong in this quad tree
    double* point = data + new_index * dimension;
    if(!boundary->containsPoint(point))
        return false;
    
    // Online update of cumulative size and center-of-mass
    cum_size++;
    double mult1 = (double) (cum_size - 1) / (double) cum_size;
    double mult2 = 1.0 / (double) cum_size;
    for(unsigned int d = 0; d < dimension; d++) center_of_mass[d] *= mult1;
    for(unsigned int d = 0; d < dimension; d++) center_of_mass[d] += mult2 * point[d];
    
    emb_density_com = mult1*emb_density_com + mult2*all_emb_dens[new_index];
    log_emb_density_com = mult1*log_emb_density_com + mult2*all_log_emb_dens[new_index];
    log_orig_density_com = mult1*log_orig_density_com + mult2*all_log_orig_dens[new_index];
    emb_density_no_entropy_com = mult1*emb_density_no_entropy_com
      + mult2 * all_emb_dens_no_entropy[new_index];     
    marg_Q_com = mult1*marg_Q_com + mult2*all_marg_Q[new_index];
    

    
    // If there is space in this quad tree and it is a leaf, add the object here
    if(is_leaf && size < QT_NODE_CAPACITY) {
        index[size] = new_index;
        size++;
        return true;
    }
    
    // Don't add duplicates for now (this is not very nice)
    bool any_duplicate = false;
    for(unsigned int n = 0; n < size; n++) {
        bool duplicate = true;
        for(unsigned int d = 0; d < dimension; d++) {
            if(point[d] != data[index[n] * dimension + d]) { duplicate = false; break; }
        }
        any_duplicate = any_duplicate | duplicate;
    }
    if(any_duplicate) return true;
    
    // Otherwise, we need to subdivide the current cell
    if(is_leaf) subdivide();
    
    // Find out where the point can be inserted
    for(unsigned int i = 0; i < no_children; i++) {
        if(children[i]->insert(new_index)) return true;
    }
    
    // Otherwise, the point cannot be inserted (this should never happen)
    return false;
}

    
// Create four children which fully divide this cell into four quads of equal area
void SPTree::subdivide() {
    
    // Create new children
    double* new_corner = (double*) malloc(dimension * sizeof(double));
    double* new_width  = (double*) malloc(dimension * sizeof(double));
    for(unsigned int i = 0; i < no_children; i++) {
        unsigned int div = 1;
        for(unsigned int d = 0; d < dimension; d++) {
            new_width[d] = .5 * boundary->getWidth(d);
            if((i / div) % 2 == 1) new_corner[d] = boundary->getCorner(d) - .5 * boundary->getWidth(d);
            else                   new_corner[d] = boundary->getCorner(d) + .5 * boundary->getWidth(d);
            div *= 2;
        }
        children[i] = new SPTree(this, dimension, data, all_emb_dens, all_log_emb_dens,
				 all_emb_dens_no_entropy, 
				 all_log_orig_dens, all_marg_Q, new_corner, new_width);
    }
    free(new_corner);
    free(new_width);
    
    // Move existing points to correct children
    for(unsigned int i = 0; i < size; i++) {
        bool success = false;
        for(unsigned int j = 0; j < no_children; j++) {
            if(!success) success = children[j]->insert(index[i]);
        }
        index[i] = -1;
    }
    
    // Empty parent node
    size = 0;
    is_leaf = false;
}


// Build SPTree on dataset
void SPTree::fill(unsigned int N)
{
    for(unsigned int i = 0; i < N; i++) insert(i);
}


// Checks whether the specified tree is correct
bool SPTree::isCorrect()
{
    for(unsigned int n = 0; n < size; n++) {
        double* point = data + index[n] * dimension;
        if(!boundary->containsPoint(point)) return false;
    }
    if(!is_leaf) {
        bool correct = true;
        for(int i = 0; i < no_children; i++) correct = correct && children[i]->isCorrect();
        return correct;
    }
    else return true;
}



// Build a list of all indices in SPTree
void SPTree::getAllIndices(unsigned int* indices)
{
    getAllIndices(indices, 0);
}


// Build a list of all indices in SPTree
unsigned int SPTree::getAllIndices(unsigned int* indices, unsigned int loc)
{
    
    // Gather indices in current quadrant
    for(unsigned int i = 0; i < size; i++) indices[loc + i] = index[i];
    loc += size;
    
    // Gather indices in children
    if(!is_leaf) {
        for(int i = 0; i < no_children; i++) loc = children[i]->getAllIndices(indices, loc);
    }
    return loc;
}


unsigned int SPTree::getDepth() {
    if(is_leaf) return 1;
    int depth = 0;
    for(unsigned int i = 0; i < no_children; i++) depth = fmax(depth, children[i]->getDepth());
    return 1 + depth;
}


// Compute non-edge forces using Barnes-Hut algorithm
void SPTree::computeDensityForces(unsigned int point_index, double theta, double dense_f1[], 
				  double dense_f2[])
{
    
    // Make sure that we spend no time on empty nodes or self-interactions
    if(cum_size == 0 || (is_leaf && size == 1 && index[0] == point_index)) return;
    
    // Compute distance between point and center-of-mass
    double D = .0;
    unsigned int ind = point_index * dimension;
    for(unsigned int d = 0; d < dimension; d++) buff[d] = data[ind + d] - center_of_mass[d];
    for(unsigned int d = 0; d < dimension; d++) D += buff[d] * buff[d];
    
    // Check whether we can use this node as a "summary"
    double max_width = 0.0;
    double cur_width;
    for(unsigned int d = 0; d < dimension; d++) {
        cur_width = boundary->getWidth(d);
        max_width = (max_width > cur_width) ? max_width : cur_width;
    }
    if(is_leaf || max_width / sqrt(D) < theta) {
    
        // Compute and add t-SNE force between point and current node
      double dist = sqrt(D); 
      double sq_dist = D; 
      D = 1.0 / (1.0 + D);
      double mult = cum_size * D / dist;

      /*
      double dr_me = D / all_marg_Q[point_index] 
	* (2*dist + (1 - sq_dist) / all_emb_dens[point_index]); 
      double dr_you = D / marg_Q_com
	* (2*dist + (1 - sq_dist) / emb_density_com); 
       */

      // dr_me is d r_{i} / d_{ij}
      double dr_me = (D * D  / (all_marg_Q[point_index] * all_emb_dens[point_index])
		      * (log( D / all_marg_Q[point_index]) * (1 - sq_dist) + 2 * sq_dist
			 + 2 * dist * all_emb_dens_no_entropy[point_index]));
      // dr_me is d r_j / d_{ij} (uses the centers of mass)
      double dr_you = (D * D / (marg_Q_com*emb_density_com)
		       * (log( D / marg_Q_com) * (1-sq_dist) + 2*sq_dist
			  + 2*dist * emb_density_no_entropy_com)); 
      
      for(unsigned int d = 0; d < dimension; d++) {
	dense_f1[d] += mult * (all_log_orig_dens[point_index]*dr_me 
			       + log_orig_density_com*dr_you) * buff[d]; 
	dense_f2[d] += mult * (all_log_emb_dens[point_index]*dr_me
			       + log_emb_density_com*dr_you) * buff[d]; 
	}


      
    }
    else {
        // Recursively apply Barnes-Hut to children
      for(unsigned int i = 0; i < no_children; i++) children[i]->computeDensityForces(point_index, theta, dense_f1, 
										      dense_f2);
    }
}

// Print out tree
void SPTree::print() 
{
    if(cum_size == 0) {
        printf("Empty node\n");
        return;
    }

    if(is_leaf) {
        printf("Leaf node; data = [");
        for(int i = 0; i < size; i++) {
            double* point = data + index[i] * dimension;
            for(int d = 0; d < dimension; d++) printf("%f, ", point[d]);
            printf(" (index = %d)", index[i]);
            if(i < size - 1) printf("\n");
            else printf("]\n");
        }        
    }
    else {
        printf("Intersection node with center-of-mass = [");
        for(int d = 0; d < dimension; d++) printf("%f, ", center_of_mass[d]);
        printf("]; children are:\n");
        for(int i = 0; i < no_children; i++) children[i]->print();
    }
}

