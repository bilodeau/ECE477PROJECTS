# read in the CSV file
temps = read.csv("temp_log", header = FALSE, sep = ",");

# create an image device on which to draw the plot
png(filename="temp_log.png",height=800,width=800,res=72);

# plot the data points
plot(0:(length(temps)-1),temps/100,ylab="Temperature (degrees Celsius)",xlab="Time (in measurement intervals)",type='o',col='black');

# add a title to the plot
title(main="Recorded Temperature Data");

# close the device
dev.off();
