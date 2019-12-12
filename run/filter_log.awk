BEGIN {
     FS = " ";
     f=1
     FieldSize[f++]=3;
     #watchers
     FieldSize[f++]=4;
     for(i=1;i<3;i++) {
        FieldSize[f++] = 3;
     }
     #smokers
     FieldSize[f++]=4;
     for(i=1;i<3;i++) {
        FieldSize[f++] = 3;
     }
     #inventory
     FieldSize[f++]=4;
     for(i=1;i<3;i++) {
        FieldSize[f++] = 3;
     }
     #cigarettes
     FieldSize[f++]=4;
     for(i=1;i<3;i++) {
        FieldSize[f++] = 3;
     }
}

/.*/ {
    if(NF==13) {
#        print  "NOTFILTE " $0
        for(i=1; i<=13; i++) {
            if(i<8) {
               if($i==prev[i]) {
                 printf("%*s ",FieldSize[i],".")
               }
               else printf ("%*s ",FieldSize[i],$i) 
               prev[i] = $i
            }
            else {
               printf ("%*s ",FieldSize[i],$i) 
            }
        }
        printf("\n")
    }
    else print $0
}

END{
}
