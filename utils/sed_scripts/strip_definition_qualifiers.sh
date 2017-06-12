sed -r \
	-e 's/<ncicp:ComplexDefinition xmlns:ncicp=http:\/\/ncicb.nci.nih.gov\/xml\/owl\/EVS\/ComplexProperties.xsd#>//g' \
	-e 's/<ncicp:def-definition>//g' \
	-e 's/<\/ncicp:def-definition>//g' \
	-e 's/<ncicp:def-source>[-A-Z]*<\/ncicp:def-source>//g' \
	-e 's/<ncicp:Definition_Reviewer_Name>[-a-z _A-Z]*<\/ncicp:Definition_Reviewer_Name>//g' \
	-e 's/<ncicp:Definition_Review_Date>[0-9]*<\/ncicp:Definition_Review_Date>//g' \
	-e 's/<ncicp:attr>[-a-z _A-Z:;\.]*<\/ncicp:attr>//g' \
	-e 's/<\/ncicp:ComplexDefinition>//g' \
	< $1 
