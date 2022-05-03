#include <bcm2835.h>
#include "motor.cpp"
#include <csignal>
#include <sys/types.h>
#include <unistd.h>
// include threading library
#include <thread>


bool isvalid=true;
//motor is a pointer to be deletable
Motor1 *motor1;
Motor2 *motor2;

void signalHandler( int signum ) {
    #ifdef DEBUG
    std::cout << "\nsignal " << signum << " triggered" <<std::endl;
    #endif
    
    //disable motor
    bcm2835_gpio_write(ENABLE, HIGH);

    // disable the run loop for motors
    motor2->valid=false;
    motor1->valid=false;
    
    // set isvalid to false
    isvalid=false;

    

    #ifdef DEBUG
    //std::cout << "Sigterm signal ended" <<std::endl;
    #endif
}

void runmotor(Motor * motor)
    {
        motor->run();
    }


int main(int argc, char **argv)
{
    std::cout << "Started ...\n";
    // print the process id
    std::cout << "PID: " << getpid() << std::endl;

    if (!bcm2835_init())
      return (1);
    unsigned int delay=00;
    int steps=0;
    bool dir=true;

    //motor is a pointer to be deletable
    motor1 = new Motor1();
    motor2 = new Motor2();

    motor1->dir=dir;
    motor2->dir=dir;
    //motor1->delay=delay;
    //motor2->delay=delay;


    // set the signal handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

 
    // run the two motors in different threads
    std::thread tm1(runmotor,motor1);
    std::thread tm2(runmotor,motor2);
    
    for (int i=1000;i>12;i*=0.99)
    {
        motor1->delay=i;
        motor2->delay=i;
        //print the delay
        std::cout << "delay: " << motor1->delay << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // if i is lower than 100, wait more
        if (i<100)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (!isvalid)
        {
            break;
        }
    }

    tm1.join();
    tm2.join();
    /*
    runmotor(motor1,isvalid,delay,steps,dir);
    isvalid=true;
    runmotor(motor2,isvalid,delay,steps,dir);
    */

    delete motor1;
    delete motor2;

    bcm2835_close();
    return 0;
}
