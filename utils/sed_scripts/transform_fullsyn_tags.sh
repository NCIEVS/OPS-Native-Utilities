sed -r \
	-e 's/<ncicp:ComplexTerm xmlns:ncicp=""http:\/\/ncicb.nci.nih.gov\/xml\/owl\/EVS\/ComplexProperties.xsd#"">//g' \
	-e 's/<ncicp:term-name>//g' \
	-e 's/<\/ncicp:term-name>//g' \
	-e 's/<ncicp:term-group>/(/g' \
	-e 's/<\/ncicp:term-group>/)/g' \
	-e 's/<ncicp:term-source>/[/g' \
	-e 's/<\/ncicp:term-source>/]/g' \
	-e 's/<ncicp:source-code>/{/g' \
	-e 's/<\/ncicp:source-code>/}/g' \
	-e 's/<\/ncicp:ComplexTerm>//g' \
	< $1 
