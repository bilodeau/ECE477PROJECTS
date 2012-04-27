x = 0:256
y1 = rep(0.5,257)
y2 = rep(0.5,257)

par(mfrow=c(2,1))
plot(x*20/256,y1,yaxp=c(0,5,2))
plot(x*2/256,y2,yaxp=c(0,5,2))