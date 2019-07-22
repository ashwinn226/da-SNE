import sys
import numpy as np
import bhtsne
from sklearn.decomposition import PCA

# data = np.loadtxt('../example_data/pollen.txt',delimiter=',').T

# data = np.log(1+data)

# data = data - np.mean(data, axis=1, keepdims=True)
# data = data/(np.sum(data**2, axis=1, keepdims=True))**.5

# pca = PCA(n_components=50)

# pc_data = pca.fit_transform(data)

# infile = sys.argv[1]
# outfile = 'bh_' + infile + '_out.txt'
# betafile = 'bh_' + infile + '_betas.txt'

infile = sys.argv[1]
# outfile = sys.argv[2]
# betafile = sys.argv[3]
if '.txt' in infile:
    infile = infile[:infile.find('.txt')]

file_root = 'bh_{}_{}.txt'

# outfile = sys.argv[2]

pc_data = np.loadtxt(infile+'.txt').T

if pc_data.shape[0] < pc_data.shape[1]:
    pc_data = pc_data.T

print(pc_data.shape)

embedded, betas, orig_densities, emb_densities = bhtsne.run_bh_tsne(pc_data, initial_dims=pc_data.shape[1], theta=0.3,
                              verbose=True, perplexity=50, max_iter=1000, use_pca=False)

# np.savetxt(outfile , embedded)
# np.savetxt(betafile, betas)
np.savetxt(file_root.format(infile, 'out'), embedded)
np.savetxt(file_root.format(infile, 'betas'), betas)
np.savetxt(file_root.format(infile, 'marg_origD'), orig_densities)
np.savetxt(file_root.format(infile, 'marg_embD'), emb_densities)
