// 3 Filters
// https://beammyselfintothefuture.wordpress.com/

/*
float lastInput[2];
float lastOutput[2];
float momentum[2];
 
float AudioDoResonantLPF(float input, uint8_t channel, uint8_t reso, uint8_t freq) {
    float distanceToGo = input - lastOutput[channel];
    momentum[channel] += distanceToGo *  (freq/500); // Lower / higher number here will lower / raise the cutoff frequency
    return lastOutput[channel] += momentum[channel] + distanceToGo * (reso/500); // Higher number here (max 1) lessens resonance
}
*/
/*


// float lastOutput[2];
// float momentum[2];
 
float AudioDoResonantHPF(float input, uint8_t channel, uint8_t reso, uint8_t freq) {
    lastOutput[channel] += momentum[channel] - lastInput[channel] + input;
    lastInput[channel] = input;
    momentum[channel] = momentum[channel] * (float) (400/reso) - lastOutput[channel] * (float) (400/freq); // First number controls resonance; second controls cutoff frequency
    return lastOutput[channel];
}


float withoutTreble[2];
float bassOnly[2];
 
float AudioDoShelvingEQ(float input, uint8_t channel, uint8_t treblefreq, uint8_t bassfreq ) {
 
    // Treble calculations
    float distanceToGo = input - withoutTreble[channel];
    withoutTreble[2] += distanceToGo * (float)(400/treblefreq); // Number controls treble frequency
    float trebleOnly = input - withoutTreble[channel];
 
    // Bass calculations
    distanceToGo = withoutTreble[channel] - bassOnly[channel];
    bassOnly[channel] += distanceToGo * (float)(400/bassfreq); // Number controls bass frequency
 
    return withoutTreble[channel] + trebleOnly * 1 + bassOnly[channel] * 0;
    // The "1" controls treble. 0 = none; 1 = untouched; 2 = +6db
    // The "0" controls bass. -1 = none; 0 = untouched; 1 = +6db
}

*/


