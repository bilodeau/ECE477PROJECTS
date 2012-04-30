png(filename="Desktop/ECE477/docs/motorcontrolwaveforms.png",height=500, width=400,res=72)

x = 0:256
y1 = c(rep(5,22),rep(0,235))
y2 = c(rep(5,44),rep(0,213))
y3 = c(rep(5,78),rep(0,179))
y4 = c(rep(5,254),rep(0,3))

par(mfrow=c(2,1))
plot(x,y1,type='l',yaxp=c(0,5,1),ylab="Output Voltage (volts)",xaxt='n',xlab="Compare Value for 50Hz Signal")
axis(1,at=c(0,22,44,256),labels=c("0","13","26","(20ms)"))
title("Motor Control Waveforms")
lines(x,y2,type='l',lty=2)
legend('topright',legend=c("0% power","100% power"),lty=c(1,2))

plot(x,y3,type='l',yaxp=c(0,5,1),ylab="Output Voltage (volts)",xaxt='n',xlab="Compare Value for 488Hz Signal")
axis(1,at=c(0,78,254,256),labels=c("0","78","254  (2ms)",""))
lines(x,y4,type='l',lty=2)
legend('bottomleft',legend=c("0%","100%"),lty=c(1,2))
dev.off()