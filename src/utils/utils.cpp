#include "utils.h"

using namespace std;
using namespace arma;

/* reimplements the getch() function available on windows
 * 
 * returns: sign take from console
 * input: -
*/
int getch() {
	struct termios oldt, newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}

std::vector<double> constructDmpMys(mat joints) {
	vector<double> ret;
	double tmax = joints(joints.n_rows - 1, 0);
	for(int i = 0; i < (tmax + 1); ++i) {
		ret.push_back(i);
	}
	return ret;
}

float* copyJoints(const float* arr, int arrSize) {
	float* ret = new float[arrSize];
	for(int i = 0; i < arrSize; ++i) ret[i] = arr[i];
	return ret;
}

void printDoubleVector(vector<double>* data) {
	int dataSize = data->size();
	for(int i = 0; i < dataSize; ++i) {
		cout << data->at(i) << ", ";
	}
	cout << endl;
}

vector<double> computeDMPMys(vector<double> mys, double ax, double tau) {
	int mysSize = mys.size();
	vector<double> dmpMys;
	for(int i = 0; i < mysSize; ++i) {
		double val = exp(-ax / tau * mys.at(i));
		dmpMys.push_back(val);
	}
	return dmpMys;
}

std::vector<double>* getDoubleVectorFromArray(double* arr, int size) {
	
	vector<double>* v = new vector<double>(arr, arr + size);
	return v;
	
}

vec computeDiscreteDerivatives(vec x, vec y) {
	
	double y0, y1, x0, x1, k;
	
	vec ret(x.n_elem);
	
	for(int i = 1; i < x.n_elem; ++i) {
		
		y0 = y(i - 1);
		y1 = y(i);
		x0 = x(i - 1);
		x1 = x(i);
		
		k = (y1 - y0) / (x1 - x0);
		ret(i - 1) = k;
	}
	
	ret(ret.n_elem - 1) = k;
	
	return ret;
	
}

string buildPolynomialEquation(double* w, int paramCount) {
	string ret = "0";
	for(int i = 0; i < paramCount; ++i) {
		std::ostringstream s;
		s << " + " << w[i] << " * x**" << i;
		ret += s.str();
	}
	return ret;
}

double* poly_eval_multiple(double* data, int data_len, double* c, int c_len) {
	double* evals = new double[data_len];
	for(int i = 0; i < data_len; ++i) {
		evals[i] = gsl_poly_eval(c, c_len, data[i]);
	}
	return evals;
}

double* polyder(double* c, int len) {
	double* dc = new double[len];
	for(int i = 1; i < len; ++i) {
		dc[i - 1] = c[i] * i;
	}
	dc[len - 1] = 0;
	return dc;
}

double* polynomialfit(int obs, int degree, double *dx, double *dy) {
	
	gsl_multifit_linear_workspace *ws;
	gsl_matrix *cov, *X;
	gsl_vector *y, *c;
	double chisq;
	double* store = new double[degree];
	
	int i, j;
	
	X = gsl_matrix_alloc(obs, degree);
	y = gsl_vector_alloc(obs);
	c = gsl_vector_alloc(degree);
	cov = gsl_matrix_alloc(degree, degree);
	
	// computation of design matrix according to bishop equation 3.16 (page 142)
	for(i = 0; i < obs; ++i) {
		gsl_matrix_set(X, i, 0, 1.0);
		for(j=0; j < degree; j++) {
			gsl_matrix_set(X, i, j, pow(dx[i], j));
		}
		gsl_vector_set(y, i, dy[i]);
	}
	
	ws = gsl_multifit_linear_alloc(obs, degree);
	gsl_multifit_linear(X, y, c, cov, &chisq, ws);
	
	for(i = 0; i < degree; ++i) {
		store[i] = gsl_vector_get(c, i);
	}
	
	gsl_multifit_linear_free(ws);
	gsl_matrix_free(X);
	gsl_matrix_free(cov);
	gsl_vector_free(y);
	gsl_vector_free(c);
	
	return store;
	
}

float* createFloatArrayFromStdVector(vector<float>* data) {
	int size = data->size();
	float* retArr = new float[size];
	for(int i = 0; i < size; ++i) retArr[i] = data->at(i);
	return retArr;
}

double* createDoubleArrayFromStdVector(vector<double>* data) {
	int size = data->size();
	double* retArr = new double[size];
	for(int i = 0; i < size; ++i) retArr[i] = data->at(i);
	return retArr;
}

double* createDoubleArrayFromArmaVector(vec data) {
	int size = data.n_elem;
	double* retArr = new double[size];
	for(int i = 0; i < size; ++i) retArr[i] = data(i);
	return retArr;
}

double* createDoubleArrayFromVector(gsl_vector* data) {
	int size = data->size;
	double* retArr = new double[size];
	for(int i = 0; i < size; ++i) retArr[i] = gsl_vector_get(data, i);
	return retArr;
}

/*
 * transforms gsl_matrix to double** matrix
 * returns: double** representation of matrix
 * input:
 *	gsl_matrix* data:	data in gsl_matrix format 
 * 
*/
double** createDoubleArrayFromMatrix(gsl_matrix* data) {
	int rows = data->size1;
	int columns = data->size2;
	double** retArr = new double*[rows];
	for(int i = 0; i < rows; ++i) {
		retArr[i] = new double[columns];
		for(int j = 0; j < columns; ++j) {
			retArr[i][j] = gsl_matrix_get(data, i, j);
		}
	}
	return retArr;
}

/*
 * releases allocated memory for double array
 * returns: -
 * input:
 * 	double** data:		double array
 * 	int columns:		number of columns in array
 * 
*/
void freeDoubleArray(double** data, int columns) {
	for(int i = 0; i < columns; ++i) {
		free(data[i]);
	}
	free(data);
}

void printDoubleVector(double* data, int size) {
	for(int i = 0; i < size; ++i) {
		cout << data[i] << ",";
	}
	cout << endl;
}

/*
 * prints a double matrix to console
 * returens: -
 * input:
 * 	double** data:		matrix
 * 	int rows:		number of rows in matrix
 * 	int columns:		number of columns in matrix
*/
void printDoubleMatrix(double** data, int rows, int columns) {
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < columns; ++j) {
			cout << data[i][j] << ",  ";
		}
		cout << endl;
	}
	fflush(stdout);
}

gsl_vector* createGslVectorFromStdVector(std::vector<double>* data) {
	gsl_vector* ret = gsl_vector_alloc(data->size());
	for(int i = 0; i < data->size(); ++i) gsl_vector_set(ret, i, data->at(i));
	return ret;
}

std::vector<double>* createStdVectorFromGslVector(gsl_vector* vec) {
	std::vector<double>* ret = new std::vector<double>();
	for(int i = 0; i < vec->size; ++i) {
		ret->push_back(gsl_vector_get(vec, i));
	}
	return ret;
}

/*
 * converts a queue array representation to the gsl_matrix format
 * returns: gsl_matrix version of data
 * input:
 * 	std:queue<double>** data:	queue array containing data
 * 	int columns:			number of columns (length of data array)
*/
gsl_matrix* createMatrixFromQueueArray(std::queue<double>** data, int columns) {
	gsl_matrix* retMatrix = gsl_matrix_alloc(data[0]->size(), columns);
	int numRows = data[0]->size();
	for(int i = 0; i < numRows; ++i) {
		for(int j = 0; j < columns; ++j) {
			double val = data[j]->front();
			data[j]->pop();
			gsl_matrix_set(retMatrix, i, j, val);
		}
	}
	return retMatrix;
}

gsl_matrix* invertSquareMatrix(gsl_matrix* mat) {
	
//	gsl_linalg_SV_decomp(
	
	int s;
	int size = mat->size1;
	gsl_matrix* retMatrix = gsl_matrix_alloc(size, size);
	gsl_permutation* p = gsl_permutation_alloc(size);
	gsl_linalg_LU_decomp (mat, p, &s);
	gsl_linalg_LU_invert(mat, p, retMatrix);
	return retMatrix;

}

/*
 * parses a string and returns the floating point value of the represented number
 * 
 * returns: number parsed from the input string s, or 0 if parsing was not successful
 * input:
 * 	const std::string& s:	string to parse
*/
double string_to_double(const std::string& s) {
	std::istringstream i(s);
	double x;
	if (!(i >> x))
		return 0;
	return x;
} 

/*
 * reads double numbers from file
 * 
 * returns: array of queues, where each queue stores a column
 * input:
 *	char* file:		complete path to input file
 *	int fileColumns:	count of columns to import
*/
mat readMovements(string file, int fileColumns) {
	
	mat joints(1, fileColumns);
	string line;
	string token;
	double dn = 0.0;
	double t0 = 0.0;
	
	ifstream inFile;
	inFile.open (file, ios::in | ios::app | ios::binary);
	if (inFile.is_open()) {
		cout << string("(utils) trajectory file ") + file + " opened\n";
		int j = 0;
		while(inFile.good()) {
			getline(inFile, line);
			Tokenizer tok(line);
			for(int i = 0; (token = tok.next()) != "" && i < fileColumns; ++i) {
				dn = string_to_double(token);
				
				// normalization
				if(j == 0 && i == 0) { t0 = dn; dn = 0; }
				else if(i == 0) dn -= t0;
				
				joints(j, i) = dn;
				
			}
			fflush(stdout);
			j++;
			joints.resize(j + 1, fileColumns);
		}
		joints.resize(j - 1, fileColumns);
	}
	
	inFile.close();
	return joints;
}

vec readQuery(string file) {
	vec ret(1);
	string line;
	string token;
	double dn = 0.0;
	int i = 0;
	ifstream inFile;
	inFile.open (file, ios::in | ios::app | ios::binary);
	if (inFile.is_open()) {
		cout << string("(utils) query file ") + file + " opened\n";
		if(inFile.good()) {
			getline(inFile, line);
			Tokenizer tok(line);
			while((token = tok.next()) != "") {
				dn = string_to_double(token);
				ret(i) = dn;
				++i;
				ret.resize(i + 1);
			}
		}
	}
	ret.resize(i);
	
	inFile.close();
	return ret;
}

vector<string> getFilesInDirectory(string folderPath) {
	
	vector<string> ret;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (folderPath.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			string tmp(ent->d_name);
			ret.push_back(tmp);
		}
		
		closedir (dir);
	}
	return ret;
}

vector<string> sortPrefix(vector<string> list, string prefix) {
	
	vector<string> ret;
	int listSize = list.size();
	
	for(int i = 0; i < listSize; ++i) {
		string currentString = list.at(i);
		int pos = currentString.find(prefix);
		if(!pos) ret.push_back(currentString);
	}
	return ret;
}

std::vector<DMPBase> buildDMPBase(vector<double> tmpmys, vector<double> tmpsigmas, double ax, double tau) {
	
	std::vector<DMPBase> baseDef;
	vector<DMPBase>::iterator it = baseDef.begin();
	
	vector<double> mys = computeDMPMys(tmpmys, ax, tau);
	
	for(int i = 0; i < mys.size(); ++i) {
		
		double realMy = tmpmys.at(i);
		double my = mys.at(i);
		
		vector<double> sigmas;
		
		for(int j = 0; j < tmpsigmas.size(); ++j) {
			double realSigma = tmpsigmas.at(j);
			double sigma = my - exp( -ax / tau * ( realMy + realSigma ) );
			sigmas.push_back(sigma);
		}
		
		DMPBase base(my, sigmas);

		// with this implementation, currently all sigmas have to be of the same size (see DMPTrajectoryGenerator::evaluateBasisFunction)
		it = baseDef.insert(it, base);
	}
	
	return baseDef;

}

mat gslToArmadilloMatrix(gsl_matrix* matrix) {
	int lines = matrix->size1;
	int columns = matrix->size2;
	mat ret = mat(lines, columns);
	for(int i = 0; i < lines; ++i)
		for(int j = 0; j < columns; ++j)
			ret(i, j) = gsl_matrix_get(matrix, i, j);
	return ret;
}

vector<double> armadilloToStdVec(vec armadilloVec) {
	vector<double> retVec;
	for(int i = 0; i < armadilloVec.n_elem; ++i)
		retVec.push_back(armadilloVec(i));
	return retVec;
}

vec stdToArmadilloVec(vector<double> stdVec) {
	vec ret(stdVec.size());
	for(int i = 0; i < stdVec.size(); ++i) {
		ret(i) = stdVec.at(i);
	}
	return ret;
}

vector<double>* testGaussianRegressor() {
	
	vector<double>* ret = new vector<double>[4];
	vector<vec> data;

	vector<double> sampleXs;
	vector<double> sampleYs;
	cout << "producing sample data" << endl;
	/*
	vec ts(20);
	for(double i = 0; i < 1; i = i + 0.05) {
		cout << "\t" << i; fflush(stdout);
		vec point(1);
		point(0) = i;
		data.push_back(point);
		ts(i) = sin(i * 6);
		
		sampleXs.push_back(i);
		sampleYs.push_back(ts(i));
	}
*/
	
/*
	vec x0(1); x0(0) = -1.5; data.push_back(x0);
	vec x1(1); x1(0) = -1.0; data.push_back(x1);
	vec x2(1); x2(0) = -0.75; data.push_back(x2);
	vec x3(1); x3(0) = -0.4; data.push_back(x3);
	vec x4(1); x4(0) = -0.25; data.push_back(x4);
	vec x5(1); x5(0) = 0.0; data.push_back(x5);
	
	vec ts(6);
	ts(0) = -1.7; ts(1) = -1.2; ts(2) = -0.35; ts(3) = 0.1; ts(4) = 0.5; ts(5) = 0.75;
	
	sampleXs.push_back(-1.5);
	sampleXs.push_back(-1.0);
	sampleXs.push_back(-0.75);
	sampleXs.push_back(-0.4);
	sampleXs.push_back(-0.25);
	sampleXs.push_back(0.0);
	
	sampleYs.push_back(-1.7);
	sampleYs.push_back(-1.2);
	sampleYs.push_back(-0.35);
	sampleYs.push_back(0.1);
	sampleYs.push_back(0.5);
	sampleYs.push_back(0.75);
*/
/*
	vec ts(15);
	int j = 0;
	for(double i = 0.0; i < 3.0; i = i + 0.2) {
		
		vec dat(1);
		dat(0) = i;
		data.push_back(dat);
		
		ts(j) = sin(2 * i);
		
		sampleXs.push_back(i);
		sampleYs.push_back(sin(2 * i));
		
		j++;
	}
*/
	vec ts(6);
	for(int i = 0; i < 6; ++i) {
		vec dat(1);
		dat(0) = i + 2;
		data.push_back(dat);
		
		sampleXs.push_back(i + 2);
	}
	
	ts(0) = -1.75145; sampleYs.push_back(-1.75145);
	ts(1) = -1.74119; sampleYs.push_back(-1.74119);
	ts(2) = -1.73314; sampleYs.push_back(-1.73314);
	ts(3) = -1.72789; sampleYs.push_back(-1.72789);
	ts(4) = -1.73026; sampleYs.push_back(-1.73026);
	ts(5) = -1.72266; sampleYs.push_back(-1.72266);
	
	vector<double> xs;
	vector<double> ys;
	GaussianProcessRegressor* reg = new GaussianProcessRegressor(data, ts, new GaussianKernel(0.5, 0.05), 10000);
//	GaussianProcessRegressor* reg = new GaussianProcessRegressor(data, ts, new GaussianKernel(0.5, 1), 10000);
//	GaussianProcessRegressor* reg = new GaussianProcessRegressor(data, ts, new TricubeKernel(), 1000);
	cout << endl << "do fitting" << endl;
	for(double i = 2.0; i < 7.0; i = i + 0.01) {
		vec query(1);
		query(0) = i;
		double res = reg->fitAtPosition(query)(0);
		xs.push_back(i);
		ys.push_back(res);
	}
	
	ret[0] = sampleXs;
	ret[1] = sampleYs;
	ret[2] = xs;
	ret[3] = ys;

	return ret;
}

arma::vec squareMatrixToColumn(arma::mat Z) {
	
	vec zCoeffs(Z.n_cols * Z.n_rows);
	
	int k = 0;
	for(int i = 0; i < Z.n_cols; ++i) {
		for(int j = 0; j < Z.n_rows; ++j) {
			double currVal = Z(i, j);
			zCoeffs(k) = currVal;
			++k;
		}
	}
	
	return zCoeffs;
	
}
arma::mat columnToSquareMatrix(arma::vec c) {
	
	int dim = sqrt(c.n_elem);
	arma::mat newM(dim, dim);
	
	int k = 0;
	for(int i = 0; i < dim; ++i) {
		for(int j = 0; j < dim; ++j) {
			newM(i, j) = c(k);
			++k;
		}
	}
	
	return newM;
}