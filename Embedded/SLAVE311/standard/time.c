


/****************************************************************************
//  Delay function, 1000 = 1 second.
****************************************************************************/
void ms_delay(unsigned int cnt) //Set a small delay so that clean packets are processed
{
  for(i=0;i<cnt;i++)
  {
  __delay_cycles(16000);  //This is for a 16MHz crystal ATMEGA128A
  }
}


/****************************************************************************
//  Timer for displaying data via UART-Serial (each second up to 99:59)
****************************************************************************/
void timer(int num)
{ 
  int ans, ans2, rem, rem2 = 0;
  printf("\n\r");  
  ans = num/60;    
  rem = num%60;   
  if (rem > 9)
   {  
     ans2 = rem/10;
     rem2 = rem%10;
   }
  else if (rem < 10)
   {
     ans2 = rem/10;
     rem2 = rem;
   }
  //printf("Elapsed Time (MM:SS) = %02d:%d%d\r\n",ans,ans2,rem2);
}


