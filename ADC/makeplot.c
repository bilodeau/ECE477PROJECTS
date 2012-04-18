
int main(){
	system("R --vanilla <generatePlot.R> r.out");
	system("rm r.out");
	return 0;
}
