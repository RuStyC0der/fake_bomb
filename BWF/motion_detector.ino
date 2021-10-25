
int motion_A_pin = 6;
int motion_B_pin = 5;


bool motion_A_detected(){
    return digitalRead(motion_A_pin);
}

bool motion_B_detected_inverted(){
    return !digitalRead(motion_A_pin);
}

bool full_motion_detected(){
    return digitalRead(motion_A_pin) && digitalRead(motion_B_pin);
}

bool particle_motion_detected(){
    return digitalRead(motion_A_pin) || digitalRead(motion_B_pin);
}

bool particle_motion_detected_inverted(){
    return !digitalRead(motion_A_pin) || !digitalRead(motion_B_pin);
}
