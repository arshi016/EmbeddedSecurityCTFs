#include "currentTime.h"

/*You have the sense of keys that are being sent over the network.
Now the robot wants to sync its time at the 5th minute.
Craft a message that will send time to the robot, 
5 minutes after this time: 0x63845361 second AND IP = 0x1111

Robot takes input from Planet White. Planet White has a navigation 
protocol and it is prone to spoofing/replay attack.
Device already left the factory.
Factory may have trouble updating the device with the new key.
Why?? Unauthenticated protocol about the location.
Commands are authenticated. Only the GPS is not.
Lots of robots were sent. Never thought that someone could use the data channel.

Suppose, enemies waiting to capture the robot near CIC.
Planet White knows that. Attack:
Sequence of replays that robot ends up getting to CIC.

Drone: 1. should go to the desired location
2. not go to the dangerous location
*/

void time_sync_attack(uint64_t* v, uint64_t key) {
    printf("\nOriginal Values:");
    printf("[ %X %X ]", v[0], v[1]);
    printf("\nkey = %lx", key);
    uint64_t v0=v[0], v1=v[1], sum=0, i;             // set up
    uint64_t delta=0x123457898765432;                // a key schedule constant
    for (i=0; i < 32; i++) {                         // basic cycle start
        sum += delta;
        v0 += ((v1<<4) + key) ^ (v1 + sum) ^ ((v1>>5) + key);
        v1 += ((v0<<4) + key) ^ (v0 + sum) ^ ((v0>>5) + key);
    }                                                // end cycle
    v[0]=v0; v[1]=v1;
    printf("\n Encrypted:       ");
    printf("[ %lX %lX ]", v[0], v[1]);
}