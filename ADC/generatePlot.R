temps = read.csv("temp_log", header = FALSE, sep = ",");

png(filename="temp_log.png",height=800,width=800,res=72);
# Compute Draw an estimated bound for the given data.
plot(0:(length(temps)-1),temps/100,ylab="Temperature (degrees Celsius)",xlab="Time (in measurement intervals)",type='o',col='black');

 title(main="Recorded Temperature Data");
dev.off();
