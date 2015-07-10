
#ifndef MODULES_EDUPICKING_WMEDUPICKINGHELPER_H_
#define MODULES_EDUPICKING_WMEDUPICKINGHELPER_H_


template <typename T>
class WMEDUColor
{
private:
	T red;
	T green;
	T blue;
	T alpha;

public:
	WMEDUColor();
	WMEDUColor(T red, T green, T fBlue, T alpha);

	//Getter
	T getRed();
	T getGreen();
	T getBlue();
	T getAlpha();

	//Setter
	void setRed(T red);
	void setGreen(T green);
	void setBlue(T blue);
	void setAlpha(T alpha);

	void normalize();
};


template <typename T>
WMEDUColor<T>::WMEDUColor()
	: WMEDUColor(0.0, 0.0, 0.0, 0.0)
{
}

template <typename T>
WMEDUColor<T>::WMEDUColor(T red, T green, T blue, T alpha)
	: red(red), green(green), blue(blue), alpha(alpha)
{
}

//Getter
template <typename T>
T WMEDUColor<T>::getRed()
{
	return this->red;
}

template <typename T>
T WMEDUColor<T>::getGreen()
{
	return this->green;
}

template <typename T>
T WMEDUColor<T>::getBlue()
{
	return this->blue;
}

template <typename T>
T WMEDUColor<T>::getAlpha()
{
	return this->alpha;
}

//Setter
template <typename T>
void WMEDUColor<T>::setRed(T red)
{
	this->red = red;
}

template <typename T>
void WMEDUColor<T>::setGreen(T green)
{
	this->green = green;
}

template <typename T>
void WMEDUColor<T>::setBlue(T blue)
{
	this->blue = blue;
}

template <typename T>
void WMEDUColor<T>::setAlpha(T alpha)
{
	this->alpha = alpha;
}

template <typename T>
void WMEDUColor<T>::normalize()
{
	this->red = this->red / 255.0;
	this->green = this->green / 255.0;
	this->blue = this->blue / 255.0;
	this->alpha = this->alpha / 255.0;
}


#endif /* MODULES_EDUPICKING_WMEDUPICKINGHELPER_H_ */
