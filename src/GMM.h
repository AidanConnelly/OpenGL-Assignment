#ifndef GMM_H
#define GMM_H

#include<vector>
#include<iostream>
#include<math.h>

const int NUMBER_OF_CLUSTERS = 16;

const double PI = 3.14159265359;
const double SQRT_2_PI = sqrt(2 * PI);


const double mili = 0.001;
const double MIN_WIDTH = 0.05 * mili * mili * mili;

const double MIN_STEP = 0.003;
const double STEP_SIZE_MULTIPLIER = 1.4;
const double MAX_STEP = 4.0;

const double SOFTMAX_WIDTH = 0.48;

struct gmm
{
	std::vector<double> means;
	std::vector<double> width;

	float uniformRandom()
	{
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	void init(int n)
	{
		for (int i = 0; i < n; i++)
		{
			means.push_back(uniformRandom() - 0.5);
			width.push_back(uniformRandom() + 0.1);
		}
	}

	double logEnt(double val, int j)
	{
		return
			- log(SQRT_2_PI * width[j])
			- (val - means[j]) * (val - means[j]) / (2 * width[j] * width[j]);
	}

	double inDistribution(double x, int distrib)
	{
		return (x - means[distrib]) / width[distrib];
	}

	double outDistribution(double x, int distrib)
	{
		return (x * width[distrib]) + means[distrib];
	}

	int gaussian(float x)
	{
		int toReturn = 0;
		for (int i = 0; i < means.size(); i++)
		{
			if (logEnt(x, i) > logEnt(x, toReturn))
			{
				toReturn = i;
			}
		}
		return toReturn;
	}

	static void calculateLogEntropies(std::vector<float> dat, int n, double* logEntropies, gmm* toCalc)
	{
		for (int i = 0; i < dat.size(); i++)
		{
			for (int j = 0; j < toCalc->means.size(); j++)
			{
				double val = dat[i];
				logEntropies[i + j * n] = toCalc->logEnt(val, j);
			}
		}
	}


	static void calculateDerivsAndMeanEntropies(std::vector<float> dat, gmm& entDeriv, int n, int* maxEntropyGaussian,
	                                            double* softMaxCoef, double* softMaxSum, double* logEntropies,
	                                            double& sumLogEnt, double& sumSoftMaxLogEnt, gmm* toCalcFor)
	{
		int m = toCalcFor->means.size();
		for (int i = 0; i < dat.size(); i++)
		{
			for (int j = 0; j < toCalcFor->means.size(); j++)
			{
				bool shouldSet = false;
				bool valid = maxEntropyGaussian[i] >= 0 && maxEntropyGaussian[i] <= m;
				if (valid)
				{
					double alternativeLogEnt = logEntropies[i + j * n];
					double currentLogEnt = logEntropies[i + n * maxEntropyGaussian[i]];
					if (alternativeLogEnt > currentLogEnt)
					{
						shouldSet = true;
					}
				}
				else
				{
					shouldSet = true;
				}
				if (shouldSet)
				{
					maxEntropyGaussian[i] = j;
				}
			}
		}

		for (int i = 0; i < n; i++)
		{
			softMaxSum[i] = 0;
		}
		for (int i = 0; i < dat.size(); i++)
		{
			for (int j = 0; j < toCalcFor->means.size(); j++)
			{
				int jMax = maxEntropyGaussian[i];
				double thisEnt = logEntropies[i + j * n];
				double maxEnt = logEntropies[i + jMax * n];
				double diff = thisEnt - maxEnt;
				double thisSoftMax = exp(diff / SOFTMAX_WIDTH);
				softMaxCoef[i + j * n] = thisSoftMax;
				softMaxSum[i] += thisSoftMax;
			}
		}

		for (int i = 0; i < dat.size(); i++)
		{
			for (int j = 0; j < toCalcFor->means.size(); j++)
			{
				if (j == maxEntropyGaussian[i])
				{
					sumLogEnt += logEntropies[i + j*n];
				}
				double softMaxDeriv = softMaxCoef[i + j * n] / softMaxSum[i];
				sumSoftMaxLogEnt += softMaxDeriv * logEntropies[i + j * n];
				double valTakeMean = dat[i] - toCalcFor->means[j];
				double thisWidth = toCalcFor->width[j];
				entDeriv.means[j] += softMaxDeriv * (valTakeMean / (thisWidth * thisWidth));
				entDeriv.width[j] += softMaxDeriv * (((valTakeMean * valTakeMean) - thisWidth * thisWidth) / (thisWidth * thisWidth * thisWidth));
			}
		}
	}

	void fit(std::vector<float> dat, int iterations)
	{
#define BEGIN_LOOP for(int i = 0; i<dat.size();i++){ for(int j = 0;j<means.size();j++){
#define END_LOOP }}

		gmm entDeriv;

		int n = dat.size();
		int m = means.size();

		for (int i = 0; i < m; i++)
		{
			entDeriv.means.push_back(0);
			entDeriv.width.push_back(0);
		}

		int* maxEntropyGaussian = new int[n];
		double* softMaxCoef = new double[n * m];
		double* softMaxSum = new double[n];
		double* logEntropies = new double[n * m];

		for (int t = 0; t < iterations; t++)
		{
			for (int i = 0; i < m; i++)
			{
				entDeriv.means[i] = 0;
				entDeriv.width[i] = 0;
			}

			//Initial entropy
			calculateLogEntropies(dat, n, logEntropies, this);


			double sumLogEnt = 0;
			double sumSoftMaxLogEnt = 0;
			calculateDerivsAndMeanEntropies(dat, entDeriv, n, maxEntropyGaussian, softMaxCoef, softMaxSum, logEntropies,
			                                sumLogEnt,
			                                sumSoftMaxLogEnt, this);
			sumLogEnt /= ((double)n);
			//sumSoftMaxLogEnt;
			if (iterations > 100) {
				if (t % (iterations / 100) == 0)
				{
					std::cout << (int)(t / ((float)(iterations / 100.0))) << "%: mean log ent: " << sumLogEnt << " nats"
						"\t" << "mean softmax log ent: " << sumSoftMaxLogEnt / ((double)n) << " nats" << std::endl;
				}
			}
			else
			{
				std::cout << (int)(t / ((float)(iterations / 100.0))) << "%: mean log ent: " << sumLogEnt << " nats"
					"\t" << "mean softmax log ent: " << sumSoftMaxLogEnt / ((double)n) << " nats" << std::endl;				
			}

			
			double stepSize = MIN_STEP;
			double lastSumLogSoftMax = sumSoftMaxLogEnt;
			gmm garbage = *this;
			gmm improved = *this;
			double thisSumLogSoftMax = 0;
			bool cont;
			do 
			{
				gmm next = *this;
				for (int j = 0; j < m; j++)
				{
					next.means[j] = means[j] + (stepSize / ((float)n)) * entDeriv.means[j];
					next.width[j] = width[j] + (stepSize / ((float)n)) * entDeriv.width[j];
					if (next.width[j] < MIN_WIDTH)
					{
						next.width[j] = MIN_WIDTH;
					}
				}
				calculateLogEntropies(dat, n, logEntropies, &next);
				calculateDerivsAndMeanEntropies(dat, garbage, n, maxEntropyGaussian, softMaxCoef, softMaxSum,
				                                logEntropies, sumLogEnt,
				                                thisSumLogSoftMax, &next);
				stepSize *= STEP_SIZE_MULTIPLIER;
				improved.means = next.means;
				improved.width = next.width;
				cont = (thisSumLogSoftMax >= lastSumLogSoftMax);
				lastSumLogSoftMax = thisSumLogSoftMax;
			} while (cont && stepSize <= MAX_STEP);
			this->means = improved.means;
			this->width = improved.width;
		}
	}
};

#endif
