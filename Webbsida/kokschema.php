<?php
class MyDB extends SQLite3{
   function __construct($stringDB){          //Funktion som öppnar önskad databas.
      $this->open($stringDB);
   }
}

$type = $_REQUEST["type"];                   //Läser in variablen type från GET-meddelandet.

if($type == "checkName"){                    //kokschema.php?type=checkName&name="namnet"
   checkName();
}
else if($type == "saveSchedule"){            //kokschema.php?type=saveSchedule&name="namnet"&array="arrayen"&total="totalatiden"
   saveSchedule();
}
else if ($type == "getSchedule"){            //kokschema.php?type=getSchedule&name="namnet"
   getSchedule();
}
else if ($type == "getList"){                //kokschema.php?type=getList
   getList();
}
else if ($type == "upLoad"){                 //kokschema.php?type=upLoad&name="namnet"&array="arrayen"&total="totalatiden "
   upLoad();
}
else if ($type == "response"){               //kokschema.php?type=response
   response();
}
else if ($type = "delete"){                  //kokschema.php?type=delete&name="namnet"
   delete();
}

function checkName(){                        //Funktion som kollar om namnet finns i databsen.
   $name = $_REQUEST["name"];
   if(preg_match("/[a-zA-ZåäöÅÄÖ0-9]+/", $name)){
      $db = new MyDB("sqltemptime.db");
      if(!$db){
         echo $db->lastErrorMsg();
      }
      $sql = <<<EOF
         SELECT name FROM boilschedule WHERE name="$name";
EOF;
      $ret = $db->query($sql);
      $row = $ret->fetchArray(SQLITE3_ASSOC);
      if($row['name']){
         echo "fail";
      } else {
         echo "success";
      }
      $db->close();
   }
   else{
      echo "Name not valid";
   }
}

function saveSchedule(){                     //Funktion som läser datan från GET-meddelandet och gör det redo för att läggas in i databasen.
   $array = json_decode($_REQUEST["array"]);
   $name = $_REQUEST["name"];
   $total = $_REQUEST["total"];
   if(preg_match("/[a-zA-ZåäöÅÄÖ0-9]+/", $name)){
      if(!empty($array)){
         $hops = "";
         $time = "";
         for($i = 0; $i < count($array); $i++){
            $hops .= "/".$array[$i][0];
            $time .= "/".$array[$i][1];
         }
         if(preg_match("/[a-zA-ZåäöÅÄÖ0-9]+/", $hops)){
            if(preg_match("/[0-9]+/", $time)){
               if (preg_match("/[0-9]+/", $total)){
                  writeData($name, $hops, $time, $total);
               }
               else{
                  echo "Hops not valid";
               }
            }
            else{
               echo "Time not valid";
            }
         }
         else{
            echo "Hops not valid";
         }
      }
   }
   else{
      echo "Name not valid";
   }
}

function getSchedule(){                      //Funktion som hämtar ett specifikt schema från databasen.
   $name = $_REQUEST["name"];
   if(preg_match("/[a-zA-ZåäöÅÄÖ0-9]+/", $name)){
      $db = new MyDB("sqltemptime.db");
      if(!$db){
         echo $db->lastErrorMsg();
      }
      $sql = <<<EOF
         SELECT * FROM boilschedule WHERE name = "$name";
EOF;
      $ret = $db->query($sql);
      $row=$ret->fetchArray();
      for ($i = 0; $i < 4; $i++){
         print($row[$i]." ");
      }
      $db->close();
   }
   else{
      echo "Name not valid";
   }
}

function getList(){                          //Funktion som tar fram en lista på alla scheman i databasen.
   $db = new MyDB("sqltemptime.db");
   if(!$db){
      echo $db->lastErrorMsg();
   }
   $sql = <<<EOF
      SELECT name FROM boilschedule;
EOF;
   $ret = $db->query($sql);
   while($row=$ret->fetchArray()){
      print($row[0]."/");
   }
   $db->close();
}

function writeData($name, $hops, $time, $total){     //Funktion som lägger in scheman i databasen.
   $db = new MyDB("sqltemptime.db");
   if(!$db){
      echo $db->lastErrorMsg();
   }
   $sql = <<<EOF
      INSERT INTO boilschedule (name,spice,time,total)
      VALUES ('$name', '$hops', '$time', '$total');
EOF;
   $ret = $db->exec($sql);
   if(!$ret){
      echo $db->lastErrorMsg();
   } else {
      echo "Data insertet\n";
   }
   $db->close();
}

function upLoad(){  
   $wElements = $_REQUEST["wElements"];  
   $mWarm= $_REQUEST["mWarm"];                           //Funktion som skickar en sträng till arduinon.
   $name = $_REQUEST["name"];                            //name,temp1,time1,temp2,time2....tempn,timen,checksum
   $array = json_decode($_REQUEST["array"]);             //checksum = strlen(name)+temp1+time1+temp2+...+tempn+timen
   $total = $_REQUEST["total"];
   if(preg_match("/[a-zA-ZåäöÅÄÖ0-9]+/", $name)){
      if (preg_match("/[0-9]+/", $total)){
         if(!empty($array)){
            $text = "boil,";
            $check = strlen($name);
            $text .= $name;
            $check += count($array);
            $check += $total;
            $temp = "";
            for($i = 0; $i < count($array); $i++){
               $temp .= ",".$array[$i][0];
               $temp .= ",".$array[$i][1];
               $check += strlen($array[$i][0]);
               $check += $array[$i][1];
            }
            $text .= "," .$wElements.",". $mWarm;
            $text .= ",".$check.",".count($array).",".$total;
            $text .= $temp.",";
            //echo $text;
            $fp = fopen("/dev/ttyACM0","w");
            if(!$fp){
               echo "Can't find /dev/ttyACM0";
               $fp = fopen("/dev/ttyACM1","w");
               if(!$fp){
                  echo "Can't find /dev/ttyACM1";
               }
            }
            fwrite($fp,$text);
            echo "Scheme sent";
            fclose($fp);
         }
         else{
            echo "Arrayen tom";
         }
      }
      else{
         echo "Total time invalid";
      }
   }
   else{
      echo "Invalid namn";
   }
}

function delete(){
   $db = new MyDB("sqltemptime.db");
   if(!$db){
      echo $db->lastErrorMsg();
   }
   $name = $_REQUEST["name"];
   $sql = <<<EOF
      DELETE FROM boilschedule
      WHERE name = "$name";
EOF;
   $ret = $db->exec($sql);
   if(!$ret){
      echo $db->lastErrorMsg();
   } else {
      echo "Row deleted";
   }
   $db->close();
}