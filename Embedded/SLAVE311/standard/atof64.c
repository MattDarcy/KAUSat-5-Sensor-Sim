

float atof64azel(const *char azel)
{
    double temp;
    temp = azel[1]*1 + azel[3]/10 + azel[4]/100 + azel[5]/1000 + azel[6]/10000;
    if (string[0] == '+')
       {
        temp =1*temp;             
       }
    else if string[0] == '-') 
       {
        temp = -1*temp    
       }
    return(temp);   
}

float atof64mag(const *char mag)
{
    double temp;
    temp = mag[1]*10000 + mag[2]*1000 + mag[3]*100 + mag[4]*10 + mag[5]*1 + mag[7]/10 + mag[8]/100;
    if (string[0] == '+')
       {
        temp =1*temp;             
       }
    else if string[0] == '-') 
       {
        temp = -1*temp    
       }
    return(temp);
}

float atof64gyro(const *char gyro)
{
    double temp;
    temp = gyro[1]*100 + gyro[2]*10 + gyro[3]*1 + gyro[5]/10 + gyro[6]/100 + gyro[7]/1000 + gyro[8]/10000;
    if (string[0] == '+')
       {
        temp =1*temp;             
       }
    else if string[0] == '-') 
       {
        temp = -1*temp    
       }   
    return(temp);
}