#ifndef GMM_H
#define GMM_H

#include<vector>
#include<stdio.h>
#include<math.h>

const int NUMBER_OF_CLUSTERS = 16;
//const int FIT_ITERATIONS = 800;
const int FIT_ITERATIONS = 8;

const float PI = 3.14159265359;
const float SQRT_2_PI = sqrt(2*PI);


const float mili = 0.001;
const float MIN_WIDTH = 0.05 * mili * mili * mili;

struct gmm{
    std::vector<float> means;
    std::vector<float> width;

    float  uniformRandom(){
        return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }

    void init(int n){
        for(int i = 0;i<n;i++){
            means.push_back(uniformRandom()-0.5);
            width.push_back(uniformRandom()+0.1);
        }
    }

	float logEnt(float val, int j){
    	return
		- log(SQRT_2_PI * width[j])
		- (val - means[j]) * (val - means[j]) / (2 * width[j] * width[j]);
	}

	float inDistribution(float x, int distrib) {
		return (x - means[distrib])/width[distrib];
	}

	float outDistribution(float x, int distrib) {
		return (x * width[distrib]) + means[distrib];
    }

	int gaussian(float x){
		int toReturn = 0;
		for(int i = 0;i<means.size();i++){
			if(logEnt(x, i)>logEnt(x,toReturn)){
				toReturn = i;
			}
		}
		return toReturn;
	}

    void fit(std::vector<float> dat){
        gmm entDeriv;
        
        int n = dat.size();
        int m = means.size();

        for(int i = 0;i<m;i++){
            entDeriv.means.push_back(0);
            entDeriv.width.push_back(0);
        }

#define BEGIN_LOOP for(int i = 0; i<dat.size();i++){ for(int j = 0;j<means.size();j++){
#define END_LOOP }}
    	
		int* maxEntropyGaussian = new int[n];
		for (int t = 0; t < FIT_ITERATIONS; t++) {

            for(int i = 0;i<m;i++){
                entDeriv.means[i] = 0;
                entDeriv.width[i] = 0;
            }

			//Initial entropy
			float* logEntropies = new float[n * m];
			BEGIN_LOOP
				float val = dat[i];
				logEntropies[i + j * n] = logEnt(val, j);
			END_LOOP

			BEGIN_LOOP
				bool shouldSet = false;
			bool valid = maxEntropyGaussian[i] >= 0 && maxEntropyGaussian[i] <= m;
			if (valid) {
				float alternativeLogEnt = logEntropies[i + j * n];
				float currentLogEnt = logEntropies[i + n * maxEntropyGaussian[i]];
				if (alternativeLogEnt > currentLogEnt) {
					shouldSet = true;
				}
			}
			else {
				shouldSet = true;
			}
			if (shouldSet) {
				maxEntropyGaussian[i] = j;
			}
			END_LOOP

			float* softMaxCoef = new float[n * m];
			float* softMaxSum = new float[n];
			for(int i = 0;i<n;i++)
			{
				softMaxSum[i] = 0;
			}
			BEGIN_LOOP
				int jMax = maxEntropyGaussian[i];
				float thisEnt = logEntropies[i + j * n];
				float maxEnt = logEntropies[i + jMax * n];
				float thisSoftMax = exp(thisEnt - maxEnt);
				softMaxCoef[i + j * n] = thisSoftMax;
				softMaxSum[i] += thisSoftMax;
			END_LOOP
			
			float sumLogEnt = 0;
			BEGIN_LOOP
				if (j == maxEntropyGaussian[i]) {
					sumLogEnt += logEntropies[i, j];
				}
				float softMaxDeriv = softMaxCoef[i + j * n] / softMaxSum[i];
				float valTakeMean = dat[i] - means[j];
				entDeriv.means[j] += softMaxDeriv*( valTakeMean / (width[j] * width[j]));
				entDeriv.width[j] += softMaxDeriv*(((valTakeMean * valTakeMean)- width[j] * width[j]) / (width[j] * width[j] * width[j]));
			END_LOOP
			sumLogEnt /= ((float)n);
			//std::cout << "mean log ent: " << sumLogEnt << std::endl;

			float stepSize = 0.18/((float) n);
			for (int j = 0; j < m; j++) {
				means[j] += stepSize * entDeriv.means[j];
				width[j] += stepSize * entDeriv.width[j];
				if(width[j]< MIN_WIDTH)
				{
					width[j] = MIN_WIDTH;
				}
			}
		}
	}
};

#endif