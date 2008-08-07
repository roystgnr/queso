#ifndef __UQ_TEMPLATE_EX_H__
#define __UQ_TEMPLATE_EX_H__

#include <uqDRAM_MarkovChainGenerator.h>

template<class V, class M>
struct
uqAppl_M2lPriorFunction_DataType
{
  const V* maybeYouNeedAVector;
  const M* maybeYouNeedAMatrix;
};

template<class V, class M>
struct
uqAppl_M2lLikelihoodFunction_DataType
{
  const V* aVectorForInstance;
  const M* aMatrixForInstance;
};

template<class V, class M>
void 
uqAppl(const uqEnvironmentClass& env)
{
  if (env.rank() == 0) {
    std::cout << "Beginning run of 'uqEtcEx' example\n"
              << std::endl;
  }

  UQ_FATAL_TEST_MACRO(env.isThereInputFile() == false,
                      env.rank(),
                      "uqAppl()",
                      "input file must be specified in command line, after the '-i' option");

  //******************************************************
  // Step 1 of 4: Define the finite dimensional linear spaces.
  //              Define the Markov chain generator.
  //******************************************************
  uqParamSpaceClass <V,M> calParamSpace (env,"cal");
  uqOutputSpaceClass<V,M> calOutputSpace(env,"cal");

  uq_M2lPriorFunction_Class<V,M>      uq_M2lPriorFunction_Obj;
  uq_M2lLikelihoodFunction_Class<V,M> uq_M2lLikelihoodFunction_Obj;
  uqDRAM_MarkovChainGeneratorClass<V,M> mcg(env,
                                            "cal",
                                            calParamSpace,
                                            calOutputSpace,
                                            &uq_M2lPriorFunction_Obj,
                                            uq_M2lLikelihoodFunction_Obj);

  //******************************************************
  // Step 2 of 4: Compute the proposal covariance matrix.
  //******************************************************
  M* proposalCovMatrix = NULL;

  //******************************************************
  // Step 3 of 4: Prepare the data to be passed to
  //              uqAppl_M2lPriorFunction_Routine() and
  //              uqAppl_M2lLikelihoodFunction_Routine().
  //******************************************************
  V paramInitials   (calParamSpace.initialValues   ());
  V paramPriorSigmas(calParamSpace.priorSigmaValues());

  uqAppl_M2lPriorFunction_DataType<V,M> uqAppl_M2lPriorFunction_Data;
  uqAppl_M2lPriorFunction_Data.maybeYouNeedAVector = &paramInitials;
  uqAppl_M2lPriorFunction_Data.maybeYouNeedAMatrix = NULL;

  uqAppl_M2lLikelihoodFunction_DataType<V,M> uqAppl_M2lLikelihoodFunction_Data;
  uqAppl_M2lLikelihoodFunction_Data.aVectorForInstance = &paramPriorSigmas;
  uqAppl_M2lLikelihoodFunction_Data.aMatrixForInstance = NULL;

  //******************************************************
  // Step 4 of 4: Generate chains.
  //              Output data is written (in MATLAB format) to the file
  //              with name specified by the user in the input file.
  //******************************************************
  mcg.generateChains(proposalCovMatrix,
                     (void *) &uqAppl_M2lPriorFunction_Data,
                     (void *) &uqAppl_M2lLikelihoodFunction_Data,
                     NULL,
                     true);

  //******************************************************
  // Release memory before leaving routine.
  //******************************************************

  if (env.rank() == 0) {
    std::cout << "Finishing run of 'uqEtcEx' example"
              << std::endl;
  }

  return;
}

template<class V, class M>
double uqAppl_M2lPriorFunction_Routine(const V& paramValues, const void* functionDataPtr)
{
  const V& v1 = *((uqAppl_M2lPriorFunction_DataType<V,M> *) functionDataPtr)->maybeYouNeedAVector;
  const M& m1 = *((uqAppl_M2lPriorFunction_DataType<V,M> *) functionDataPtr)->maybeYouNeedAMatrix;

  UQ_FATAL_TEST_MACRO(true,
                      paramValues.env().rank(),
                      "uqAppl_M2lPriorFunction_Routine(), in uqEtcEx.h",
                      "should not be here, since application is using the default prior() routine provided by PECOS toolkit");

  return (m1 * v1).sumOfComponents();
}

template<class V, class M>
void uqAppl_M2lLikelihoodFunction_Routine(const V& paramValues, const void* functionDataPtr, V& resultValues)
{
  const V& v1 = *((uqAppl_M2lLikelihoodFunction_DataType<V,M> *) functionDataPtr)->aVectorForInstance;
  const M& m1 = *((uqAppl_M2lLikelihoodFunction_DataType<V,M> *) functionDataPtr)->aMatrixForInstance;

  resultValues[0] = (m1 * v1).norm2Sq();

  return;
}
#endif // __UQ_TEMPLATE_EX_H__