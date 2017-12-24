# Association-rules-generation-using-Frequent-Pattern-Mining
program takes four parameters: (i) the minimum support, (ii) the minimum confidence, (iii) the name of the input file, and (iv) the name of the output file.
The output format of each line will be as follows:
LHS|RHS|SUPPORT|CONFIDENCE
Both LHS and RHS will contain the items that make up the left- and right-hand side of the rule in a space delimited fashion.
Due to the large number of the generated association rules, for support values of <=20 only frequent itemsets are generated

Steps to run: 

1)make fptminer

2)./fptminer minsup minconf inputfile outputfile
