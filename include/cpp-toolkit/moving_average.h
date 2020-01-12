#ifndef MovingAverage_h
#define MovingAverage_h

template<class T, size_t window_size>
class MovingAverage
{
public:
  MovingAverage(): sum(0), index(0) {}

  T get() const { return sum / window_size; }
  T push(T value)
  {
    sum += value - data[index];
    data[index++] = value;
    if (index >= window_size)
    {
      index = 0;
    }
    return get();
  }
private:
  T data[window_size];
  T sum;
  size_t index;
};

#endif
