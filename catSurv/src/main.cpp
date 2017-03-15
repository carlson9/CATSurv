#include <Rcpp.h>
#include "Cat.h"
#include <boost/variant.hpp>
using namespace Rcpp;

/**
 * To enable the usage of R-style methods (i.e. functions that take their object as their first parameter),
 * this file contains a direct mapping from the functions called by RCpp to methods of the Cat class.
 * In other words, every method in this file converts an S4 Cat object to a C++ Cat object, then calls the appropriate
 * method of the C++ Cat object.
 */

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::depends(RcppArmadillo)]]

//' Probability of Responses to a Question Item or the Left-Cumulative Probability of Responses
//'
//' Calculates the probability of specific responses or the left-cumulative probability of responses to \code{item} conditioned on a subject's ability (\eqn{\theta})  
//'
//' @param catObj An object of class \code{Cat}
//' @param theta A numeric or an integer indicating the value for \eqn{\theta_j}
//' @param item An integer indicating the index of the question item
//'
//' @return When the argument \code{catObj} is an \code{ltm} model, the function \code{probabilty} returns a numeric vector of length one representing the probabilty of observing a non-zero response.
//'
//'When the argument \code{catObj} is an \code{tpm} model, the function \code{probabilty} returns a numeric vector of length one representing the probabilty of observing a non-zero response.
//'
//' When the argument \code{catObj} is a \code{grm} model, the function \code{probabilty} returns a numeric vector of length k+1, where k is the number of possible responses. The first element will always be zero and the kth element will always be one. The middle elements are the cumulative probability of observing response k or lower.
//'
//'  When the argument \code{catObj} is a \code{gpcm} model, the function \code{probabilty} returns a numeric vector of length k, where k is the number of possible responses. Each number represents are the probability of observing response k.
//'
//' @details 
//'  For the \code{ltm} model, the probability of non-zero response for respondent \eqn{j} on item \eqn{i} is
//'  
//'  \deqn{Pr(y_{ij}=1|\theta_j)=\frac{\exp(a_i + b_i \theta_j)}{1+\exp(a_i + b_i \theta_j)}}
//'
//'  where \eqn{\theta_j} is respondent \eqn{j} 's position on the latent scale of interest, \eqn{a_i} is item \eqn{i} 's discrimination parameter,
//'  \eqn{b_i} is item \eqn{i} 's difficulty parameter, and \eqn{c_i} is item \eqn{i} 's guessing parameter. 
//'  
//'  For the \code{tpm} model, the probability of non-zero response for respondent \eqn{j} on item \eqn{i} is
//'  
//'  \deqn{Pr(y_{ij}=1|\theta_j)=c_i+(1-c_i)\frac{\exp(a_i + b_i \theta_j)}{1+\exp(a_i + b_i \theta_j)}}
//'
//'  where \eqn{\theta_j} is respondent \eqn{j} 's position on the latent scale of interest, \eqn{a_i} is item \eqn{i} 's discrimination parameter,
//'  \eqn{b_i} is item \eqn{i} 's difficulty parameter, and \eqn{c_i} is item \eqn{i} 's guessing parameter. 
//'  
//'  For the \code{grm} model, the probability of a response in category \eqn{k} \strong{or lower} for respondent \eqn{j} on item \eqn{i} is
//' 
//'  \deqn{Pr(y_{ij} <  k|\theta_j)=\frac{\exp(\alpha_{ik} - \beta_i \theta_{ij})}{1+\exp(\alpha_{ik} - \beta_i \theta_{ij})}}{Pr(y_ij < k | \theta_j) = (exp(\alpha_ik - \beta_i \theta_ij))/(1 + exp(\alpha_ik - \beta_i \theta_ij))}
//'
//'  where \eqn{\theta_j} is respondent \eqn{j} 's position on the latent scale of interest, \eqn{\alpha_ik} the \eqn{k}-th element of item \eqn{i} 's difficulty parameter, 
//'  \eqn{\beta_i} is discrimination parameter vector for item \eqn{i}. Notice the inequality on the left side and the absence of guessing parameters.
//'
//'  For the \code{gpcm} model, the probability of a response in category \eqn{k} for respondent \eqn{j} on item \eqn{i} is
//' 
//'  \deqn{Pr(y_{ij} =  k|\theta_j)=\frac{\exp(\sum_{t=1}^k \alpha_{i} [\theta_j - (\beta_i - \tau_{it})])}
//'  {\sum_{r=1}^{K_i}\exp(\sum_{t=1}^{r} \alpha_{i} [\theta_j - (\beta_i - \tau_{it}) )}}
//'  
//'  
//'  where \eqn{\theta_j} is respondent \eqn{j} 's position on the latent scale of interest, \eqn{\alpha_i} is the discrimination parameter for item \eqn{i},
//'  \eqn{\beta_i} is the difficulty parameter for item \eqn{i}, and \eqn{\tau_{it}} is the category \eqn{t} threshold parameter for item \eqn{i}, with \eqn{k = 1,...,K_i} response options
//'  for item \eqn{i}.  For identification purposes \eqn{\tau_{i0} = 0} and \eqn{\sum_{t=1}^1 \alpha_{i} [\theta_j - (\beta_i - \tau_{it})] = 0}.
//'
//'@examples
//'\dontrun{
//'## Probability for Cat object of the ltm model
//'data(npi)
//'cat <- ltmCat(npi)
//'probability(cat, theta = 1, item = 1)
//'
//'## Probability for Cat object of the tpm model
//'data(polknow)
//'cat <- tpmCat(polknow)
//'probability(cat, theta = 1, item = 1)
//'
//'## Probability for Cat object of the grm model
//'data(nfc)
//'cat <- grmCat(nfc)
//'probability(cat, theta = 1, item = 1)
//'}
//'  
//' @seealso \code{\link{Cat}} for information on the item parameters: discrimination, difficulty, and guessing.
//'  
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//' 
//' @references 
//' Baker, Frank B. and Seock-Ho Kim. 2004. Item Response Theory: Parameter Estimation Techniques. New York: Marcel Dekker.
//' 
//' Choi, Seung W. and Richard J. Swartz. 2009. “Comparison of CAT Item Selection Criteria for Polytomous Items.” Applied Psychological Measurement 33(6):419–440.
//' 
//' Muraki, Eiji. 1992. "A generalized partial credit model: Application of an EM algorithm." ETS Research Report Series 1992(1): 1-30.
//' 
//' van der Linden, Wim J. 1998. “Bayesian Item Selection Criteria for Adaptive Testing.” Psychometrika 63(2):201–216.
//' 
//'  
//' @export
// [[Rcpp::export]]
std::vector<double> probability(S4 catObj, NumericVector theta, IntegerVector item) {
	Cat cat = Cat(catObj);
	double t = theta[0];
	int q = item[0];
	if(q == 0){
	  throw std::domain_error("Must use an item number applicable to Cat object.");
	}
	return cat.probability(t, q);
}

//' Likelihood of the Specified Response Set
//'
//' Calculates the likelihood of a respondent, with ability parameter \eqn{\theta}, having offered the specific set of responses stored in the \code{Cat} objects \code{answers} slot, conditional on the item-level parameters.
//'
//' @param catObj An object of class \code{Cat}
//' @param theta A numeric or an integer indicating the value for \eqn{\theta_j} 
//' 
//' @return The function returns a numeric value of the likelihood of the respondent having offered the provided response profile.
//'
//' @details
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//' 
//' @references 
//' 
//'@examples
//'\dontrun{
//'## Likelihood for Cat object of the ltm model
//'data(npi)
//'cat <- ltmCat(npi)
//'setAnswers(cat) <- c(1,0,1,0, rep(NA, 35))
//'likelihood(cat, theta = 1)
//'}
//' 
//'  
//' @seealso \code{\link{probability}} for probability of responses to individual question item
//'  
//' @export
// [[Rcpp::export]]
double likelihood(S4 catObj, double theta) {
	return Cat(catObj).likelihood(theta);
}



//' Prior Value
//'
//' Calculates the density at \code{x} of either the normal, student's t, or uniform distribution
//'
//' @param x A numeric value at which to evaluate the prior
//' @param dist A string indicating the distribution (slot \code{priorName} of \code{Cat} object)
//' @param params A length two numeric vector indicating the parameters of the distribution (slot \code{priorParams} of \code{Cat} object)
//' 
//' @return A scalar consisting of prior value, \eqn{\pi(x)}, given the value \eqn{x}
//'
//' @details The \eqn{dist} argument needs to be either "UNIFORM", "NORMAL", or "STUDENT_T".
//' 
//' When \eqn{dist} is "NORMAL", the first element of \eqn{params} is the mean, 
//' the second element is the standard deviation.  When \eqn{dist} is "STUDENT_T", the first 
//' element of \eqn{params} is the non-centrality parameters and the second is degrees of freedom.  
//' When \eqn{dist} is "UNIFORM", the elements of \eqn{params} are the lower and upper bounds,
//' of the interval, respectively.  Note that the "UNIFORM" is only applicable for the "EAP" estimation method.   
//' 
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'
//'cat@priorName <- "NORMAL"
//'cat@priorParams <- c(0, 1) ## Parameters are mean and standard deviation
//'prior(x = 1, cat@priorName, cat@priorParams)
//'
//'cat@priorName <- "STUDENT_T"
//'cat@priorParams <- c(1, 3) ## Parameters are non-centrality param and degrees of freedom
//'prior(x = 1, cat@priorName, cat@priorParams)
//'
//'cat@priorName <- "UNIFORM"
//'cat@priorParams <- c(-1, 1) ## Parameters are lower bound and upper bound of interval
//'prior(x = 1, cat@priorName, cat@priorParams)
//'}
//'
//' @seealso \code{\link{Cat}} for information on \code{priorName} and \code{priorParams} slots
//'  
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//' 
//' This function uses Boost C++ source libraries for the uniform and Student's t
//' distributions and calls \code{dnorm4} written in C which is identical to that 
//' of \code{dnorm} in \code{R}.
//' 
//'  
//'  
//' @export
// [[Rcpp::export]]
double prior(NumericVector x, CharacterVector dist, NumericVector params) {
  std::string name = Rcpp::as<std::string>(dist);
  std::vector<double> args = Rcpp::as<std::vector<double> >(params);
  return Prior(name, args).prior(x[0]);
}

//' The first derivative of the log-likelihood
//' 
//' When \code{usePrior = FALSE}, this function evaluates the first derivative of the log-likelihood evaluated at point \eqn{\theta}.  
//' When \code{usePrior = TRUE}, this function evaluates the first derivative of the log-posterior evaluated at point \eqn{\theta}. 
//' 
//' @param catObj An object of class \code{Cat}
//' @param theta A numeric or an integer indicating the value for \eqn{\theta_j}
//' @param use_prior A logical indicating whether to use the prior parameters in estimation
//' 
//' @return The function returns a numeric of the derivative of the log-likelihood (or log-posterior) for a respondent's answer profile.
//' 
//' @details This method is only be available using the normal prior distribution 
//'
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//' 
//' @seealso \code{\link{Cat}} and \code{\link{prior}} for information on available priors and prior specifications
//'  
//' @export
// [[Rcpp::export]]
double dLL(S4 &catObj, double theta, bool use_prior){
  return Cat(catObj).dLL(theta, use_prior);
}

//' The second derivative of the log likelihood
//'
//'When \code{usePrior = FALSE}, this function evaluates the second derivative of the log-likelihood evaluated at point \eqn{\theta}.  
//'When \code{usePrior = TRUE}, this function evaluates the second derivative of the log-posterior evaluated at point \eqn{\theta}. 
//'
//' @param catObj An object of class \code{Cat}
//' @param theta A numeric or an integer indicating the value for \eqn{\theta_j}
//' @param use_prior A logical indicating whether to use the prior parameters in estimation
//' 
//' @return The function returns a numeric of the second derivative of the log-likelihood (or log-posterior) for a respondent's answer profile.
//' 
//' @details This method is only be available using the normal prior distribution
//' 
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//' 
//' @seealso \code{\link{Cat}} and \code{\link{prior}} for information on available priors and prior specifications
//'  
//' @export
// [[Rcpp::export]]
double d2LL(S4 &catObj, double theta, bool use_prior){
  return Cat(catObj).d2LL(theta, use_prior);
}

//' Estimate of the respondent's ability parameter
//'
//' Calculates the expected value of the ability parameter \eqn{\theta}, conditioned on the observed answers and the item calibrations
//'
//' @param catObj An object of class \code{Cat}
//'
//' @return A numeric consisting of the expected value of the ability parameter
//'
//' @details
//' EAP
//' MAP (This method is only available using the normal prior distribution.)
//' The maximum likelihood (MLE) approach: (Note: When MLE will not work, \code{estimateTheta} is calculated according to the \code{estimationDefault} slot in \code{Cat} object.)
//' The weighted maximum likelihood approach (WLE) approach: Estimating \eqn{\theta_j} requires finding the root of the following function using the ``Brent'' method in the \code{gsl} library.
//' 
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//' 
//' @seealso \code{\link{probability}} and \code{\link{likelihood}}
//'  
//' @export
// [[Rcpp::export]]
double estimateTheta(S4 catObj) {
	return Cat(catObj).estimateTheta();
}

//' Observed Information
//'
//' Calculates the observed information of the likelihood evaluated at the input value \eqn{\theta} for a specific item
//'
//' @param catObj An object of class \code{Cat}
//' @param theta A numeric or an integer indicating the value for \eqn{\theta_j}
//' @param item An integer indicating the index of the question item
//'
//' @return The function returns a numeric value of the observed information of the likelihood, given \eqn{\theta}, for a specific question item
//' 
//' @details The observed information is equivalent to the negative second derivative of the log-likelihood.
//'   Note: This function should never be called when the respondent has answered no questions.
//'   
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//'
//' @seealso \code{\link{estimateTheta}} for calculation of \eqn{\theta} and
//'   \code{\link{expectedObsInf}} for further application of observed information
//'  
//' @export
// [[Rcpp::export]]
double obsInf(S4 catObj, double theta, int item) {
  item = item - 1;
  return Cat(catObj).obsInf(theta, item);
}

//' Expected Observed Information
//'
//' Calculates the expected information, which is the observed information attained from a specific response set times the probability of that profile occurring
//'
//' @param catObj An object of class \code{Cat}
//' @param item An integer indicating the index of the question item
//' 
//' @return The function returns a numeric value of the expected information 
//' 
//' @details
//' 
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//'
//' @seealso \code{\link{estimateTheta}} for calculation of \eqn{\theta} and 
//'   \code{\link{obsInf}} for observed information calculation
//'  
//' @export
// [[Rcpp::export]]
double expectedObsInf(S4 catObj, int item) {
  item = item - 1;
  return Cat(catObj).expectedObsInf(item);
}

//' Fisher's Information
//'
//' Calculates the expected value of the observed information of the likelihood evaluated at the input value \eqn{\theta}
//'
//' @param catObj An object of class \code{Cat}
//' @param theta A numeric or an integer indicating the potential value for \eqn{\theta_j}
//' @param item An integer indicating the index of the question item
//'
//' @return The function returns a numeric of the expected value of the observed information
//' 
//' @details For the dichotomous case, this is equivalent to the observed information.  
//' 
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//'
//' @seealso \code{\link{obsInf}} for observed information calculation and
//'   \code{\link{fisherTestInfo}} for further application of Fisher's information
//'  
//' @export
// [[Rcpp::export]]
double fisherInf(S4 catObj, double theta, int item) {
  item = item - 1;
  return Cat(catObj).fisherInf(theta, item);
}

//' Fisher's Test Information
//'
//' Calculates the total information gained for a respondent \eqn{j} for all answered items, conditioned on \eqn{theta}.
//'
//' @param catObj An object of class \code{Cat}
//' 
//' @return The total information gained for a respondent, given a specific answer set and a value of \eqn{theta}.
//' 
//' @details
//' 
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//'
//' @seealso \code{\link{fisherInf}} for calculation of Fisher's information for an individual question item
//'  
//' @export
// [[Rcpp::export]]
double fisherTestInfo(S4 catObj) {
  return Cat(catObj).fisherTestInfo();
}

//' Standard error of the respondent's ability parameter estimate
//'
//' Estimates the standard error for the ability parameter estimate
//'
//' @param catObj An object of class \code{Cat}
//'
//' @return The function returns a numeric for the standard error for \eqn{\theta_j}
//'
//' @details 
//'   The EAP estimator:
//'   The MAP estimator (This is implemented only for the normal prior.)
//'   The MLE  estimator (When MLE can't be calculated... estimationDefault)
//'   The WLE estimator (Brent method)
//'   
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//'
//' @seealso \code{\link{estimateTheta}} for estimation of \eqn{\theta}
//'  
//' @export
// [[Rcpp::export]]
double estimateSE(S4 catObj) {
	return Cat(catObj).estimateSE();
}

//' Expected Posterior Variance
//'
//' Estimates the expected posterior variance for a respondent's estimated ability parameter for an item yet to be answered based on a respondent's ability parameter estimate from the already-answered items
//'
//' @param catObj An object of class \code{Cat}
//' @param item An integer indicating the index of the question item
//'
//' @return A numeric value indicating a respondent's expected posterior variance for a yet to be asked question item
//'
//' @details 
//' 
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//'
//' 
//' @seealso \code{\link{probability}} for probability of responses to individual question item
//'   \code{\link{estimateTheta}} for estimation of \eqn{\theta}
//'  
//' 
//' @export
// [[Rcpp::export]]
double expectedPV(S4 catObj, int item) {
  item = item - 1.0;
  return Cat(catObj).expectedPV(item);
}

//' Select the next item in the question set
//'
//' Selects the next item in the question set based on the specified method
//' 
//' @param catObj An object of class \code{Cat}
//'
//' @return It returns a list with two elements: 
//' (1) A dataframe containing a column with the indexes of unasked questions and a column with the values (calculated by the specified selection method) for those items, 
//' and (2) a numeric containing the index of the question that should be asked next.
//'
//' @details The EPV method:
//'   
//'   This function takes in a \code{Cat} object from \code{R} and constructs the \code{C++} representation. It then calculates the expected posterior variance for each unanswered item. 
//'   
//'   The function returns a list with the following two elements:
//'   \code{all.estimates}: A data fame with two columns. The first column ("questions") should be the index of the question items and the second column ("EPV") of the expected posterior variance for that corresponding item. 
//'   There are as many rows in this data frame as there are unsanswered questions in the battery.
//'   \code{next.item}: A numeric vector with the index of the item with the lowest EPV value.
//'   
//'   See \code{expectedPV} for mathematical details.
//'   
//'   The MFI method:
//'   
//'   This function takes a \code{Cat} object and calculates Fisher's information for each unanswered item. It then finds the one item that maximizes Fisher's information, based on the respondent's position on the latent trait from the answered items. 
//'   
//'   The function returns a list with the following two elements:
//'   \code{all.estimates}: A data fame with two columns. The first column ("questions") should be the index of the question items and the second column of the expected posterior variance for that corresponding item.
//'   There are as many rows in this data frame as there are unsanswered questions in the battery.
//'   \code{next.item}: A numeric vector with the index of the item with the highest Fisher's information value.
//'
//'   See \code{fisherInf} for mathematical details.
//'   
//'   The MLWI method:
//'   
//'   This function calculates the likelihood for each value of X at the input value of \eqn{\theta}.
//'   Evaluates the integral over a measure of the plausibility of possible values of \eqn{\theta} by weighting Fisher's information with the likelihood function and selecting the next question according to:
//'  
//'   
//'   The MPWI method:
//'   
//'   This function calculates the likelihood for each value of X at the input value of \eqn{\theta}.
//'   Evaluates the integral over a measure of the plausibility of possible values of \eqn{\theta} by weighting Fisher's information with the likelihood function and selecting the next question according to:
//'
//' 
//'   The MEI method:
//'   
//'   This function estimates the expected observed information for a respondent’s estimated
//'   position on the latent trait on an unanswered item based on the
//'   respondent’s position on the latent trait calculated from answered items.
//'   
//'   The output should be a single numeric value.
//'
//'   Binary details:
//'
//'
//'   Categorical details:
//'   
//'   
//'   The KL method:
//'   
//'   This procedure chooses the next item with the largest KL value.
//'   
//'   See \code{expectedKL}, \code{likelihoodKL}, and/or \code{posteriorKL} for mathematical details.
//'   
//'   The MFII method:
//'   
//'   This approach chooses items based on the Fisher's information in an interval near the current estimate \eqn{\hat{\theta}}.
//'   
//'   \deqn{FII_i = \int^{\hat{\theta}+\delta}_{\hat{\theta}-\delta}I_i(\theta_0) d\theta_0}{FII_i = \int^{\hat{\theta} + \delta}_{\hat{\theta} - \delta} I_i(\theta_0) d\theta_0}
//'
//'   where \deqn{\delta = z(I(\hat{\theta}))^{-1/2}}{\delta = z(I(\hat{\theta}))^{-1/2}},  \eqn{I(\hat{\theta})} is the test information for respondent \eqn{j} evaluated at \eqn{\hat{\theta}},  \deqn{I_i(\cdot)}{I_i(.)} is the Fisher's information for item \eqn{i}, and \eqn{z} is a user specified z-value.
//'   
//'   The random method:
//'   
//'   This routine serves as a baseline for comparison. The routine simply selects an unanswered question at random.
//'
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//' 
//' @seealso \code{\link{estimateTheta}} for calculation of \eqn{\theta};  
//'   \code{\link{obsInf}} for observed information calculation;
//'   \code{\link{fisherTestInfo}} for Fisher's information calculation;
//'   \code{\link{expctedKL}} for expected Kullback-Leibeler calculation;
//'   \code{\link{likelihoodKL}} for likelihood Kullback-Leibeler calculation;  
//'   \code{\link{posteriorKL}} for posterior Kullback-Leibeler calculation; 
//'  
//' @export
// [[Rcpp::export]]
List selectItem(S4 catObj) {
  return Cat(catObj).selectItem();
}

//' Expected Kullback-Leibeler information
//'
//' Calculates the expected Kullback-Leibeler information for an individual question item
//' 
//' @return The function returns a numeric indicating the KL information for the desired item, given the current answer profile and ability parameter estimate
//' 
//' @param catObj An object of class \code{Cat}
//' @param item An integer indicating the index of the question item
//'
//' @details 
//'  Binary details (Due to the conditional independence assumption, we only need to calculate the expected value for potential new items.)
//' 
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//' 
//' @seealso \code{\link{likelihoodKL}} and/or \code{\link{posteriorKL}} for alternative KL methods
//'
//' @export
// [[Rcpp::export]]
double expectedKL(S4 catObj, int item) {
  item = item - 1;
  return Cat(catObj).expectedKL(item);
}

//' Expected Kullback-Leibeler information, weighted by the likelihood
//'
//' Calculate the expected Kullback-Leibeler information, weighted by the likelihood
//' 
//' @return A value indicating the LKL information for the desired item, given the current answer profile and ability estimate.
//' 
//' @param catObj An object of class \code{Cat}
//' @param item An integer indicating the index of the question item
//'
//' @details The LKL calculation follows the same procedure as \code{expectedKL}, except it requires weighting the different potential values of \eqn{\theta_0} by the likelihood.
//'  Thus, the equation is
//'
//'  Binary details:
//'  
//'  Due to the conditional independence assumption, we only need to calculate the expected value for potential new items.
//'  
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//' 
//'  
//' @seealso \code{\link{expectedKL}} and/or \code{\link{posteriorKL}} for alternative KL methods 
//'  
//' @export
// [[Rcpp::export]]
double likelihoodKL(S4 catObj, int item) {
  item = item - 1;
  return Cat(catObj).likelihoodKL(item);
}

//' Expected Kullback-Leibeler information, weighted by the posterior
//'
//' Calculate the expected Kullback-Leibeler information, weighted by the posterior
//' 
//' @return A value indicating the posterior KL information for the desired item, given the current answer profile and ability estimate.
//' 
//' @param catObj An object of class \code{Cat}
//' @param item An integer indicating the index of the question item
//'
//' @details We will follow the same procedure as \code{expectedKL}, except we will weight the different potential values of \eqn{\theta_0} by the posterior.
//'
//'Due to the conditional independence assumption, we only need to calculate the expected value for potential new items.
//'
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//' 
//' @seealso \code{\link{likelihoodKL}} and/or \code{\link{expectedKL}} for alternative KL methods
//' 
//' @export
// [[Rcpp::export]]
double posteriorKL(S4 catObj, int item) {
  item = item - 1;
  return Cat(catObj).posteriorKL(item);
}

//' Look Ahead to Select Next Item
//'
//' Selects the next item that would be asked for all possible response options to the question the respondent is currently answering
//'
//' @param catObj  An object of class \code{Cat}
//' @param item A numeric indicating the question item the respondent is currently answering.
//'
//' @return A vector of values indicating the possible subsequent questions
//' 
//' @examples
//' \dontrun{
//'## Prior calculation using Cat object of the ltm model
//'## specifying different distributions
//'data(npi)
//'cat <- ltmCat(npi)
//'}
//' 
//' @author Haley Acevedo, Ryden Butler, Josh W. Cutler, Matt Malis, Jacob M. Montgomery,
//'  Tom Wilkinson, Erin Rossiter, Min Hee Seo, Alex Weil 
//'  
//' @note This function is to allow users to access the internal functions of the package. During item selection, all calculations are done in compiled C++ code.
//' 
//' @seealso \code{\link{selectItem}} for selection method information
//'
//' @export
// [[Rcpp::export]]
List lookAhead(S4 catObj, int item) {
  item = item - 1.0;
  return Cat(catObj).lookAhead(item);
}

//' Check if Stop and/or Override Rules are Met
//'
//' Evaluates the specified stopping and/or override rules to check if respondent should be asked further questions
//'
//' @param catObj  An object of class \code{Cat}
//'
//' @return This function returns a boolean, where TRUE indicates the the stopping rules are met (no further questions should be asked) and FALSE indicates the stoppings rules are not met (additional questions are needed)
//'
//' @details The stopping rule thresholds are stored in the following Cat object slots: lengthThreshold, seThreshold, infoThreshold, and gainThreshold.
//'   The override thresholds are stored in the following Cat object slots: lengthOverride, gainOverride. A value of NA indicates the rule should not be used.
//' 
//'   A return value of TRUE indicates that additional questions should be asked; FALSE indicates no additional questions should be asked.
//' 
//'   A user can specify any combination of stopping rules and/or overrides.  The function returns TRUE if all specified stopping rules are met
//'   and no specified overrides are met. The function returns FALSE if at least one specified stopping rule is not met, or if any specified override threshold is met.
//' 
//'   Stopping Rules:
//'
//'   lengthThreshold: Number of question's answered >= a
//'  
//'   seThreshold: \eqn{SE(\hat{\theta}) < a}
//'  
//'   infoThreshold: \deqn{FI < a \forall}{FI < a \forall} remaining items
//'
//'   gainThreshold: \deqn{SE(\hat{\theta}) - \sqrt{EPV} | < a \forall}{SE(\hat{\theta}) - \sqrt{EPV} | < a \forall} remaining items
//'
//'   Overrides:
//'
//'   lengthOverride: Number of question's answered < a
//'
//'   gainOverride: \deqn{|SE(\hat{\theta}) - \sqrt{EPV} | >= a \forall}{|SE(\hat{\theta}) - \sqrt{EPV} | >= a \forall} remaining items
//' 
//' 
//' @export
// [[Rcpp::export]]
bool checkStopRules(S4 catObj) {
	std::vector<bool> answer = Cat(catObj).checkStopRules();
  return answer[0];
}




