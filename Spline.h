#ifndef _SPLINE_H
#define	_SPLINE_H

#include <cmath>

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
                
                for(int i=1; i < n -1; i++)
                {
                        float lambda = (x[i+1]-x[i]) / (x[i+1]-x[i-1]);
                        float mi = 1 - lambda;
                        float d = 6*( (y[i+1]-y[i]) / (x[i+1]-x[i]) - (y[i]-y[i-1]) / (x[i]-x[i-1]) ) / (x[i+1]-x[i-1]);
                        
                        q[i] = -lambda/(mi * q[i-1] + 2);
                        u[i] = (d - mi * u[i-1]) / (mi * q[i-1] + 2);
                }
                
                float d = 6*(ypn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]))/(x[n-1]-x[n-2]);
                M[n-1] = (d-u[n-2])/(q[n-2]+2);
                
                for(int i = n-2; i>=0; i--)
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
                
                q = new float[n-1];
                u = new float[n-1];
                M = new float[n];
                
                computeM();
                globalMax(NULL, NULL);
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
                float w = (( (M[i+1]-M[i]) / (6*dx)*dt+M[i]/2 )*dt +  (y[i+1]-y[i])/dx-(M[i+1]+2*M[i])/6*dx  )*dt +y[i];
                
                return w;
        }
        
        void globalMax(float *x_out, float *y_out)
        {
            //a = (M[i+1] - M[i])/(2 * (x[i+1] - x[i]))
            //b = M[i]
            //c = (y[i+1] - y[i])/(x[i+1] - x[i]) - (M[i+1] + 2*M[i])/6*(x[i+1] - x[i])
            //s'(x) = a*(x - x[i])^2 + b*(x - x[i]) + c, dla x = <x[i],x[i+1]>
            
            for(int i=0; i<=n; i++)
            {
                printf("M[%d] = %f\n",i, M[i]);
            }
            
            for(int i=0; i< n-1; i++)
            {
            
                float a = (M[i+1] - M[i])/(2 * (x[i+1] - x[i]));
                float b = M[i];
                float c = (y[i+1] - y[i])/(x[i+1] - x[i]) - (M[i+1] + 2*M[i])/6*(x[i+1] - x[i]);
            
                float delta = pow(b,2) - 4.0*a*c;
                
                printf("%d %f %s\n",i,delta, delta==0.0?"tak":"nie");
                
            }
        }
};


#endif

