//Version 1.10

#include <iostream>
#include <bitset>
#include <bits/stdc++.h>
//for tasks
#include <future>>

using namespace std;

class I2CDevice
{
    private:
        uint8_t bus=1; //the bus. Default=1 for rpi
        uint8_t device=32; //the address of the device ( eg 0x32)

    public:
        //empty constructor
        I2CDevice()
        {
//            printf("Hello world\n");
        }
        //constructor --> please pass the bus
        //number (1) and the address of the device ( eg 0x32)
        I2CDevice(uint8_t bus, uint8_t device)
        {
            bus=1;
            device=32;
            printf("initialized\n");
        }
        //takes argument a pointer to a uint8_t and the total mumber of bytes to access
        bool write_n_bytes(uint8_t* buffer, uint8_t total_bytes_including_address_byte)
        {
            printf("Passed %s",buffer);
            return true;
        };


        //takes argument a pointer to a uint8_t and the total mumber of bytes to access ~I2CDevice();
        bool read_Registers(uint8_t number, uint8_t* data)
        {
            //when fully implemented this should loop the response, which is the 2nd and 3rd byte from a 3 byte array response
            printf("Passed number:%u data:%s",number,data);
            return true;
        };
};

class Channel
{
    public:
        //The target output voltage of the reg. This value will be written to the DAC through i2c write command
        float target_output;

        //Voltage Reference Selection bit:
        //0 = VDD
        //1 = Internal voltage reference (2.048V). See datasheet
        bool Vref;

        //Gain selection bit:
        //0 = x1 (gain of 1)
        //1 = x2 (gain of 2)
        bool Gain;

        bool PD1;
        bool PD0;

        //This value is to be further decoded to PD1, PD2.
        //enable_channel =1 → (PD1,PD0)=(0,0)
        //enable_channel =0 → (PD1,PD0)=(1,1)
        bool enable_channel;

        //enable DAC: enable_channel = True
    public:
        void set_mode_on()
        {
            enable_channel=true;
            PD1=false;
            PD0=false;
        }

        //enable DAC: enable_channel = False
        void set_mode_off()
        {
            enable_channel=false;
            PD1=true;
            PD0=true;
        }

        bool get_pd_mode()
        {
            return enable_channel;//(enable_channel)
        }

        //Sets the internal variable “target_output”
        //TODO:says it's bool shouldn't it be void?
        void set_channel_output(float target_output)
        {
            this->target_output =target_output;
        }
        float get_target_output()
        {
            return this->target_output;
        }
        int get_gain()
        {
            //returns the gain parameter (see datasheet)
            return 1;
        }
};

class DAC
{
    public:
        Channel channel[4];
//        I2CDevice i2cdevice();
    public:
        DAC() // default constructor
        {
//            i2cdevice=I2CDevice(1,32);
        }
        //This command is calling the set_channel_output()
        //function for the specific channel.
        void set_output(float value,int channel_id)
        {
            //(0<channel_id <5)
            if(channel_id<0 || channel_id>5) return;

            channel[channel_id].set_channel_output(value);
            printf("Setting value :%f on channel[%d]\n",value,channel_id);
        }

        //This commands may follow a “set_output” command and is
        //used to ask the rpi to transfer the “target_output”
        //value to the MCP4728’s channel.
        //        Please use
        //• i2c write command: I2C.write_n_bytes
        //• SINGLE_WRITE (fig 5-10 of datasheet)

        char get_character_from_int(int value)
        {
            if(value==1)
            {
                return '1';
            }
            else
            {
                return '0';
            }
        }

        bool write_output(int channel_id)
        {
            //1st byte = 1100 A2 A1 A0 0
            //2nd byte = 01011 DAC DA0 not(UDAC)
            //3rd byte = Vref PD1 PD0 Gx D11 D10 D9 D8
            //4th byte = D7 D6 D5 D4 D3 D2 D1 D0
            char bit_channel[]="00";
            if(channel_id==1)
            {
                bit_channel[0]='0';
                bit_channel[1]='0';

            }
            else if(channel_id==2)
            {
                bit_channel[0]='0';
                bit_channel[1]='1';

            }
            else if(channel_id==3)
            {
                bit_channel[0]='1';
                bit_channel[1]='0';

            }
            else
            {
                bit_channel[0]='1';
                bit_channel[1]='1';
            }
            char a[32]="";

            a[0]='1';
            a[1]='1';
            a[2]='0';
            a[3]='1';
            a[4]='0';//A2
            a[5]='1';//A1
            a[6]='0';//A0
            a[7]='0';//R/not W
            a[8]='0';   //C2
            a[9]='1';   //C1
            a[10]='0';  //C0
            a[11]='1';  //W1
            a[12]='1';  //W0
            a[13]=bit_channel[0];  //DAC1
            a[14]=bit_channel[1];  //DAC0
            a[15]='0';  //not UDAC
            a[16]='0';    //Vref=0=Vdd, Vref=1=reference
            a[17]=get_character_from_int(channel[channel_id].PD1);   //PD1
            a[18]=get_character_from_int(channel[channel_id].PD0);   //PD0
            a[19]='0';    //Gx
            a[20]='0';    //D11
            a[21]='0';    //D10
            a[22]='0';    //D09
            a[23]='0';
            a[24]='0';
            a[25]='0';
            a[26]='0';
            a[27]='0';
            a[28]='0';
            a[29]='0';
            a[30]='0';    //D1
            a[31]='1';    //D0
//            a[32]='\n';

            bitset<32> bset1;
            cout<<"a:"<<a<<endl;
            for(int i=0;i<sizeof(a);i++)
            {
                if(a[i]=='1')
                {
                    printf("a[%d]:%c\n",i,a[i]);
                    bset1[sizeof(a)-i-1]=1;
                }
                else
                {
                    printf("a[%d]:%c\n",i,a[i]);
                    bset1[sizeof(a)-i-1]=0;
                }
            }
            uint8_t * buffer = (uint8_t *)a;


            cout<<"passing buffer: "<<buffer<<endl;

            I2CDevice i2c_device;
            return i2c_device.write_n_bytes(buffer,4);

            //TODO:
//            channel[channel_id].get_target_output();
        }
        bool read_registers()
        {
            char a[32]="";

            a[0]='1';
            a[1]='1';
            a[2]='0';
            a[3]='1';
            a[4]='0';//A2
            a[5]='1';//A1
            a[6]='0';//A0
            a[7]='1';//R/not W

            bitset<32> bset1;
            cout<<"a:"<<a<<endl;
            for(int i=0;i<sizeof(a);i++)
            {
                if(a[i]=='1')
                {
//                    printf("a[%d]:%c\n",i,a[i]);
                    bset1[sizeof(a)-i-1]=1;
                }
                else
                {
//                    printf("a[%d]:%c\n",i,a[i]);
                    bset1[sizeof(a)-i-1]=0;
                }
            }
            uint8_t * buffer = (uint8_t *)a;

            cout<<"reading buffer: "<<buffer<<endl;


            I2CDevice i2c_device;
            return i2c_device.read_Registers(1,buffer);
        }
};
typedef long int longint;
longint FindOdd(longint start,longint end)
{
    longint OddSum=0;
    for(longint i=start;i<=end;i++)
    {
        if(i&1)
        {
            OddSum+=i;
        }
    }
    return OddSum;
}
//test func
//float Task(int interval, std::function<void(void)> func)
//{
//    std::thread([this, interval, func]()
//    {
//        //run indefinitely
//        while (true) {
//            func();
//            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
//            printf("Printing");
//            return 2.0f;
//        }
//    }).detach();
//}

float VoltageSignal(int interval)
{
    bool execute=true;
    //max voltage
    float MaxVx=255.0f;
    //current voltage
    float Vx=0.0f;
    //times are in milliseconds
    //start increment
    float Tstart=1000.0f;
    //growth interval
    float Tr=4000.0f;
    //decrease interval
    float Tf=1000.0f;
    //total signal period
    float Tp=Tstart+Tr+Tf;

    float time=0.0f;

    float increment,decrease=0.0f;

    while(execute)
    {
        if(time<Tstart)
        {
            Vx=0.0f;
        }
        //increase signal slowly
        else if(time>Tstart && time<=(Tstart+Tr))
        {
            increment=MaxVx/(Tr/interval);
            Vx=Vx+increment;
        }
        //decrease signal fast
        else if(time>(Tstart+Tr) && time<=Tp)
        {
            decrease=MaxVx/(Tf/interval);
            Vx=Vx-decrease;
        }
        else if(time>Tp)
        {
            time=0.0f;
        }
        //wait for milliseconds using chrono
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));

        cout<<"Vx:"<<Vx<<" time:"<<time<<endl;
        //increase time
        time+=(float)interval;
    }
    return Vx;
}
int main()
{
    //channel to get the result
    std::future<float> future_result= std::async(VoltageSignal,250);

    //for future communication with the main thread
//    float x=future_result.get();

    DAC mydac;

    mydac.set_output(1.2,3);
    mydac.write_output(3);
    mydac.read_registers();


    return 0;
}

//testing stuff
//    std::thread t1(factorial,10,100);
//    t1.join();

////        int x=0;
//        int &y=x;
//        cout << "Hello world!" << endl;
//        //
//        printf("y= %d\n",y);
//        printf("GetTest= %d\n",GetTest());
//
//    Person person;
//    cout<<"Person name:"<< person.name;
