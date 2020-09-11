#include "ViewImage.h"
using namespace autopilot;


ViewImage * ViewImage::createFromFile(QString _path, int _ID)
{
	ViewImage* vi = new ViewImage();
	vi->setID(_ID);
	return vi;
}

ViewImage * autopilot::ViewImage::createFromMat(int _ID, cv::Mat _img)
{
	ViewImage* vi = new ViewImage();
	vi->setID(_ID);
	vi->image = _img;
	return vi;
}

void autopilot::ViewImage::setImage(cv::Mat img)
{
}

cv::Mat autopilot::ViewImage::getImage()
{
	return cv::Mat();
}

void autopilot::ViewImage::showImage()
{
}

int autopilot::ViewImage::getID()
{
	return 0;
}

void autopilot::ViewImage::setID(int _ID)
{
}

QPointF autopilot::ViewImage::getPosition()
{
	return QPointF();
}

void autopilot::ViewImage::setPosition(QPointF pos)
{
}

void autopilot::ViewImage::setRotation(float rotation)
{
	this->rotation = rotation;
}

ViewVector autopilot::ViewImage::compare(ViewImage img2)
{
	return ViewVector();
}



void ViewImage::setStatus(ImageStatus sta)
{
	this->status = sta;
	switch (sta)
	{
	case OutOfRange:
		this->setPixmap(QPixmap(path + "photoPosGrey.png"));
		break;
	case Detected:
		this->setPixmap(QPixmap(path + "photoPosBlue.png"));
		break;
	case ErrorPos:
		this->setPixmap(QPixmap(path + "photoPosRed.png"));
		break;
	default:
		break;
	}
}

ImageStatus autopilot::ViewImage::getStatus()
{
	return ImageStatus();
}

void autopilot::ViewImage::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
}


