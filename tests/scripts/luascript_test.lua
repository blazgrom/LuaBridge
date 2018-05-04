--Get testing
boolt_var=true;
boolf_var=false;
string_var="This is some string";
integer_var=100;
double_var=102.0351;
double_var2=103.0351;
nil_var=nil;
Position={
    x=1,
    y=2,
    z=3
};

function single_return (i)
    return i;
end
function multiple_returns(a,b)
    return a,b;
end

TableLevelOne={
    x=false,
    TableLevelTwo={
        x=true,    
        TableLevelThree={
            x=false,
            TableLevelFour={
                x=true,    
                TableLevelFive={
                    x=false,
                    TableLevelSix={
                        x=true,    
                        TableLevelSeven={
                            x=false,
                            TableLevelEight={
                                x=true,    
                                TableLevelNine={
                                    x=false,
                                    TableLevelTen={
                                        x=true,    
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}