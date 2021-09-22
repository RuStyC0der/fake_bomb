
int motion_A_pin = 6;
int motion_B_pin = 5;


bool full_motion_detected(){
    return digitalRead(motion_A_pin) && digitalRead(motion_B_pin);
}

bool particle_motion_detected(){
    return digitalRead(motion_A_pin) || digitalRead(motion_B_pin);
}

