TRUNK=../../

vpath %.h $(TRUNK)/mem/
vpath %.cpp $(TRUNK)/mem/

INCL= -I ./ -I $(TRUNK)/common/

CFLAGS= -O3 -Wall

miss_rate_sim: cache_tag_array.o miss_rate_sim.o
	$(CXX) -o $@ $^
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $^ $(INCL)

clean:
	@-rm *.o *.csv
	@-rm miss_rate_sim
