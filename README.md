# NicaFemtoMacros
Macros for nicafemto framework. In principle contains pairs of macros that can be used to estimate detector performance. First type of macro is macro for given analysis e.g. primary_efficiency.C second macro starts with "show_" and contains results of analysis.
For example to look at efficiency of reconstructed tracks user should run primary_effiency.C to create needed plots and later run show_primary_efficiency.C to plot needed plots.
## QA Core Manager
This is an abstract class that base on NicaQACoreManager it's used widely in this macro. This class represents information about format of data and cuts used by given experiment.
By default framework_def.h contains method GetCoreManager that should return QACoreManager proper to your framework.
## Core macros
### efficiency
#### primary_efficiency
This is a macro that estimate maximal efficiency of reconstructed charged pions, charged kaons and (anti) protons. The efficiency is defined as a: number of reconstructed primary particles of given type divided by total number of primary particles of given type. Efficiency is ploted as a function of transverse momentum and pseudorapidity.
#### primary_efficiency2
This is a macro that estimate maximal efficiency of reconstructed charged pions, charged kaons and (anti) protons. In contrast to primary_efficiency macro takes into account only reconstructed tracks that passed track cuts (not all reconstructed tracks). Efficiency is ploted as a function of transverse momentum and pseudorapidity.
##$primary
This is macro used to estimation of cuts for selection of primary particles. This macro presents as function of DCAxy/DCAz DCAxy-cut/DCAz-cut the fraction of primary particles, the total number of accepted primary particles and a purity of particles as a function of DCA.
#$#purity
macro for optimization of m2(p) cut. It represents the purity of charged hadrons as a function of m2(p-signed), also presents similar plot after application of PID-cuts, presents particle purity as function of signed momentum.
