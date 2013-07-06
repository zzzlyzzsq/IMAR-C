#include "IplImagePyramid.h"

// namespaces
using std::cerr;
using std::cout;
using std::endl;

void IplImagePyramid::init(IplImageWrapper image, double scaleFactor)
{
	// compute the epsilon
	_epsilon = scaleFactor * 0.05;

	// get the maximum number of levels given the scale factor
	std::size_t nLevels = 1;
	if (scaleFactor > 1.01)
		nLevels = static_cast<std::size_t>(
				floor(log(std::min(image->width, image->height) - log(1)) / log(scaleFactor)));
//if (1 > nLevels)
//	cerr << "EROR: nLevels: " << nLevels << " image size: " << image->width << "x" << image->height << " scaleFactor: " << scaleFactor << endl;
	assert(1 <= nLevels);

	// build up all levels
	std::vector<IplImageWrapper> imgPyramid(nLevels);
	std::vector<double> correctScaleFactors(nLevels);
	std::vector<double> correctScaleFactorsInv(nLevels);
	std::vector<double> correctXScaleFactors(nLevels);
	std::vector<double> correctXScaleFactorsInv(nLevels);
	std::vector<double> correctYScaleFactors(nLevels);
	std::vector<double> correctYScaleFactorsInv(nLevels);
	imgPyramid[0] = image;
	correctScaleFactors[0] = 1;
	correctScaleFactorsInv[0] = 1;
	correctXScaleFactors[0] = 1;
	correctXScaleFactorsInv[0] = 1;
	correctYScaleFactors[0] = 1;
	correctYScaleFactorsInv[0] = 1;
	for (std::size_t i = 1; i < nLevels; ++i) {
		// get the image from the last and the current scale level
		IplImageWrapper oldImg = image;
		if (i > 1)
			oldImg = imgPyramid[i - 2];
		IplImageWrapper& newImg = imgPyramid[i];

		// scale the image from the last level to the wished size
		double newScaleFactor = pow(scaleFactor, i);
		CvSize newSize = cvSize(static_cast<int>(round(image->width / newScaleFactor)),
				static_cast<int>(round(image->height / newScaleFactor)));
		newImg = IplImageWrapper(newSize, image->depth, image->nChannels);
		// TODO: different resizing scheme .. maybe renice the original image?
		cvResize(image, newImg, CV_INTER_AREA);

		// get the real scale factors
		double xScaleFactor = double(image->width) / double(newImg->width);
		double yScaleFactor = double(image->height) / double(newImg->height);
		correctXScaleFactors[i] = xScaleFactor;
		correctXScaleFactorsInv[i] = 1 / xScaleFactor;
		correctYScaleFactors[i] = yScaleFactor;
		correctYScaleFactorsInv[i] = 1 / yScaleFactor;
		correctScaleFactors[i] = 0.5 * (xScaleFactor + yScaleFactor);
		correctScaleFactorsInv[i] = 1 / correctScaleFactors[i];

		// scale the mask as well, if it exists
		if (image.hasMask()) {
			Box<int> newMask(image.getMask());
			newMask.scale(correctXScaleFactorsInv[i], correctYScaleFactorsInv[i]);
			newImg.setMask(newMask);
		}
	}

	// done
	_imagePyramid = imgPyramid;
	_scaleFactors = correctScaleFactors;
	_scaleFactorsInv = correctScaleFactorsInv;
	_xScaleFactors = correctXScaleFactors;
	_xScaleFactorsInv = correctXScaleFactorsInv;
	_yScaleFactors = correctYScaleFactors;
	_yScaleFactorsInv = correctYScaleFactorsInv;
	if (1 == nLevels)
		_scaleFactor = 0;
	else
		_scaleFactor = scaleFactor;
}

void IplImagePyramid::init(CvSize initSize, int depth, int nChannels, double scaleFactor)
{
	// compute the epsilon
	_epsilon = scaleFactor * 0.05;

	// get the maximum number of levels given the scale factor
	std::size_t nLevels = 1;
	if (scaleFactor > 1)
		nLevels = static_cast<std::size_t>(
				floor(log(std::min(initSize.width, initSize.height) - log(1)) / log(scaleFactor)));
//if (1 > nLevels)
//	cerr << "EROR: nLevels: " << nLevels << " image size: " << initSize.width << "x" << initSize.height << " scaleFactor: " << scaleFactor << endl;
	assert(1 <= nLevels);

	// build up all levels
	std::vector<IplImageWrapper> imgPyramid(nLevels);
	std::vector<double> correctScaleFactors(nLevels);
	std::vector<double> correctScaleFactorsInv(nLevels);
	std::vector<double> correctXScaleFactors(nLevels);
	std::vector<double> correctXScaleFactorsInv(nLevels);
	std::vector<double> correctYScaleFactors(nLevels);
	std::vector<double> correctYScaleFactorsInv(nLevels);
	imgPyramid[0] = IplImageWrapper(initSize, depth, nChannels);
	correctScaleFactors[0] = 1;
	correctScaleFactorsInv[0] = 1;
	correctXScaleFactors[0] = 1;
	correctXScaleFactorsInv[0] = 1;
	correctYScaleFactors[0] = 1;
	correctYScaleFactorsInv[0] = 1;
	cvSetZero(imgPyramid[0]);
	for (std::size_t i = 1; i < nLevels; ++i) {
		// get the image from the last and the current scale level
		IplImageWrapper& newImg = imgPyramid[i];

		// scale the image from the last level to the wished size
		double newScaleFactor = pow(scaleFactor, i);
		CvSize newSize = cvSize(static_cast<int>(round(initSize.width / newScaleFactor)),
				static_cast<int>(round(initSize.height / newScaleFactor)));
		newImg = IplImageWrapper(newSize, depth, nChannels);
		cvSetZero(newImg);

		// get the real scale factors
		double xScaleFactor = double(initSize.width) / double(newSize.width);
		double yScaleFactor = double(initSize.height) / double(newSize.height);
		correctXScaleFactors[i] = xScaleFactor;
		correctXScaleFactorsInv[i] = 1 / xScaleFactor;
		correctYScaleFactors[i] = yScaleFactor;
		correctYScaleFactorsInv[i] = 1 / yScaleFactor;
		correctScaleFactors[i] = 0.5 * (xScaleFactor + yScaleFactor);
		correctScaleFactorsInv[i] = 1 / correctScaleFactors[i];
	}

	// done
	_imagePyramid = imgPyramid;
	_scaleFactors = correctScaleFactors;
	_scaleFactorsInv = correctScaleFactorsInv;
	_xScaleFactors = correctXScaleFactors;
	_xScaleFactorsInv = correctXScaleFactorsInv;
	_yScaleFactors = correctYScaleFactors;
	_yScaleFactorsInv = correctYScaleFactorsInv;
	if (1 == nLevels)
		_scaleFactor = 0;
	else
		_scaleFactor = scaleFactor;
}

std::size_t IplImagePyramid::getIndex(double scaleFactor, int round) const
{
	// make sure that we don't break for the two extreme cases:
	// scale factor too large and scale factor too small
	if (scaleFactor >= _scaleFactors[_scaleFactors.size() - 1])
		return _scaleFactors.size() - 1;
	if (scaleFactor <= 1)
		return 0;

	// find the correct answer based on the given rounding type
	std::size_t i = 0;
	if (round < 0) {
		// find the next level with a smaller factor
		scaleFactor += _epsilon;
		for (i = 1; scaleFactor > _scaleFactors[i] && i < _scaleFactors.size(); ++i);
		--i;
	}
	else if (round > 0) {
		// find the next level with a bigger factor
		scaleFactor -= _epsilon;
		for (i = 0; scaleFactor >= _scaleFactors[i] && i < _scaleFactors.size() - 1; ++i);
	}
	else {
		// find the closest level with respect to the scale factor
		double bestDist = fabs(_scaleFactors[0] - scaleFactor);
		std::size_t iBest = 0;
		for (i = 1; i < _scaleFactors.size(); ++i) {
			double dist = fabs(_scaleFactors[i] - scaleFactor);
			if (dist < bestDist) {
				iBest = i;
				bestDist = dist;
			}
			else
				break;
		}
		i = iBest;
	}
	return i;
}

void IplImagePyramid::rebuild(IplImageWrapper image)
{
	if (image->width != _imagePyramid[0]->width 
			|| image->height != _imagePyramid[0]->height 
			|| image->nChannels != _imagePyramid[0]->nChannels)
		throw std::runtime_error("IplImageWrapper::rebuild() : given image dimensions and original image dimensions differ!");
	_imagePyramid[0] = image;
	for (std::size_t i = 1; i < _imagePyramid.size(); ++i)
		cvResize(image, _imagePyramid[i], CV_INTER_AREA);
}
//inline
IplImagePyramid::IplImagePyramid()
	: _imagePyramid(), _scaleFactors(), _scaleFactorsInv(), _xScaleFactors(), _xScaleFactorsInv(),
	_yScaleFactors(), _yScaleFactorsInv(), _scaleFactor(0), _epsilon(0)
{ }

//inline
IplImagePyramid::IplImagePyramid(const IplImagePyramid& pyramid)
	: _imagePyramid(pyramid._imagePyramid.begin(), pyramid._imagePyramid.end()),
	_scaleFactors(pyramid._scaleFactors.begin(), pyramid._scaleFactors.end()),
	_scaleFactorsInv(pyramid._scaleFactorsInv.begin(), pyramid._scaleFactorsInv.end()),
	_xScaleFactors(pyramid._xScaleFactors.begin(), pyramid._xScaleFactors.end()),
	_xScaleFactorsInv(pyramid._xScaleFactorsInv.begin(), pyramid._xScaleFactorsInv.end()),
	_yScaleFactors(pyramid._yScaleFactors.begin(), pyramid._yScaleFactors.end()),
	_yScaleFactorsInv(pyramid._yScaleFactorsInv.begin(), pyramid._yScaleFactorsInv.end()),
	_scaleFactor(pyramid._scaleFactor), _epsilon(pyramid._epsilon)
{ }

//inline
IplImagePyramid::IplImagePyramid(IplImageWrapper image, double scaleFactor)
{
	assert(image);
	init(image, scaleFactor);
}

//inline
IplImagePyramid::IplImagePyramid(CvSize initSize, int depth, int nChannels, double scaleFactor)
{
	init(initSize, depth, nChannels, scaleFactor);
}

//inline
IplImagePyramid::~IplImagePyramid()
{
	// everything is destroyed automatically :)
}

//inline
IplImagePyramid& IplImagePyramid::operator=(const IplImagePyramid& pyramid)
{
	_imagePyramid.clear();
	_imagePyramid = pyramid._imagePyramid;
	_scaleFactor = pyramid._scaleFactor;
	_epsilon = pyramid._epsilon;
	_scaleFactors = pyramid._scaleFactors;
	_scaleFactorsInv = pyramid._scaleFactorsInv;
	_xScaleFactors = pyramid._xScaleFactors;
	_xScaleFactorsInv = pyramid._xScaleFactorsInv;
	_yScaleFactors = pyramid._yScaleFactors;
	_yScaleFactorsInv = pyramid._yScaleFactorsInv;
	return *this;
}

//inline
IplImagePyramid::operator const bool() const
{
	return _imagePyramid.size() > 0;
}

//inline
IplImagePyramid::operator bool()
{
	return _imagePyramid.size() > 0;
}

//inline
std::size_t IplImagePyramid::numOfLevels() const
{
	return _imagePyramid.size();
}

//inline
double IplImagePyramid::getScaleFactor() const
{
	return _scaleFactor;
}

//inline
double IplImagePyramid::getScaleFactor(std::size_t index) const
{
	assert(index < _scaleFactors.size());
	return _scaleFactors[index];
}

//inline
double IplImagePyramid::getScaleFactorInv(std::size_t index) const
{
	assert(index < _scaleFactors.size());
	return _scaleFactorsInv[index];
}

//inline
double IplImagePyramid::getXScaleFactor(std::size_t index) const
{
	assert(index < _scaleFactors.size());
	return _xScaleFactors[index];
}

//inline
double IplImagePyramid::getXScaleFactorInv(std::size_t index) const
{
	assert(index < _scaleFactors.size());
	return _xScaleFactorsInv[index];
}

//inline
double IplImagePyramid::getYScaleFactor(std::size_t index) const
{
	assert(index < _scaleFactors.size());
	return _yScaleFactors[index];
}

//inline
double IplImagePyramid::getYScaleFactorInv(std::size_t index) const
{
	assert(index < _scaleFactors.size());
	return _yScaleFactorsInv[index];
}

//inline
IplImageWrapper& IplImagePyramid::getImage(std::size_t index)
{
	assert(index < _imagePyramid.size());
	return _imagePyramid[index];
}

//inline
const IplImageWrapper& IplImagePyramid::getImage(std::size_t index) const
{
	assert(index < _imagePyramid.size());
	return _imagePyramid[index];
}

//inline
IplImageWrapper IplImagePyramid::getImage(double scaleFactor, int round)
{
	return getImage(getIndex(scaleFactor, round));
}

//inline
const IplImageWrapper& IplImagePyramid::getImage(double scaleFactor, int round) const
{
	return getImage(getIndex(scaleFactor, round));
}
