#ifndef _SPLINE_H
#define	_SPLINE_H


class Spline{
        float *x, *y;
        int n;
        
        float *M; //M[i] = s''(xi)
        float *q, *u;
        
        static float const yp0 = 0.0; //y'(x0)
        static float const ypn = 0.0; //y'(xn)
        
        void computeM()
        {
                q[0] = -0.5;
                u[0] = 3*((y[1]-y[0])/(x[1]-x[0])-yp0)/(x[1]-x[0]);
                
                for(int i=1; i<n; i++)
                {
                        float lambda = (x[i+1]-x[i]) / (x[i+1]-x[i-1]);
                        float mi = 1 - lambda;
                        float d = 6*( (y[i+1]-y[i]) / (x[i+1]-x[i]) - (y[i]-y[i-1]) / (x[i]-x[i-1]) ) / (x[i+1]-x[i-1]);
                        
                        q[i] = -lambda/(mi * q[i-1] + 2);
                        u[i] = (d - mi * u[i-1]) / (mi * q[i-1] + 2);
                }
                
                float d = 6*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]))/(x[n]-x[n-1]);
                M[n] = (d-u[n-1])/(q[n-1]+2);
                
                for(int i = n-1; i>=0; i--)
                {
                        M[i] = q[i]*M[i+1]+u[i];
                }
        }
        
public:
        Spline(float *x, float *y, int n)
        {
                this->x = x;
                this->y = y;
                this->n = n;
                
                q = new float[n];
                u = new float[n];
                M = new float[n+1];
                
                computeM();
        }
        
        ~Spline()
        {
                delete []u;
                delete []q;
                delete []M;
        }
        
        //returns S(t)
        float getS(float t)
        {
                int i = n;
                float dt;
                do
                {
                        i--;
                        dt = t - x[i];
                } while(i>0 && dt<0);
                
                float dx = x[i+1] - x[i];
                float w = (( (M[i+1]-M[i]) / (6*dx)*dt+M[i]/2 )*dt + ( (y[i+1]-y[i])/dx-(M[i+1]+2*M[i])/6*dx ) )*dt +y[i];
                
                return w;
        }
};


#endif

