source('temp_log');

#  These are the coefficients for the O(log N) bound line, feel free to change.
#	They are in order of data structure types.
ocoeff = c(2000,1500,2400,1500,1500,1500);


colors = c("red","green","blue","orange","cyan","purple");
lt = c(1,2,3,4,5,6);
x = 200:xlimit;
ylimit = c(25000,20000,40000,20000,20000,20000);
# Close all open plots
graphics.off()
k = 1;
for (structure in structures){
	# Create a new plot window.
	dev.new();
	plot.new();
	# Compute Draw an estimated bound for the given data.
	plot(x,ocoeff[k]*log(x),ylab="runtime in nanoseconds",xlab="N",type='l',ylim=c(0,ylimit[k]),xlim=c(0,xlimit),col='black');
	j = 1;
	
	## Handle the special methods for MinHeap
	if((structure == "MinHeap")||(structure == "SkewHeap")){
		ops = c("insert","deleteMin");
	}else{
		ops = methods;
	}


	for(sequence in sequences){
		i = 1;
		for (op in ops){		
		sizes = eval(parse(text = paste(structure,sequence,op,"Sizes",sep="")));
		times = eval(parse(text = paste(structure,sequence,op,"Times",sep="")));
		d = data.frame(times=times,sizes=sizes);

		## Plot data points	
		points(d$sizes,d$times,col=colors[i]);

		# sort points by sizes
		d = d[order(d$sizes),];
		# throw out points that are way off of the chart before fitting
		d$times = pmin(d$times,rep(ylimit[k],length(d$times)));
		d$times = pmax(d$times,rep(0,length(d$times)));
		#d = d[floor(length(d$times)*1):length(d$times),]
		fit = lm(d$times ~ log(d$sizes));
		y = predict(fit);
		lines(d$sizes,y,col=colors[i],lty=lt[j]);

		i = i + 1;
		}
	j = j + 1;
	}
legend("bottomright",
	legend=c(ops,sequences),
	col=c(colors[1:length(ops)],rep("black",length(ops))),
	lty=c(rep(1,length(ops)),lt));
 title(main=structure);
filename = paste(tolower(structure),".png",sep="");
 dev.copy(png,filename,height=800,width=800,res=72);
 dev.off();
 k = k + 1;
}
