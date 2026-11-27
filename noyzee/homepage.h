// To create literal strings stored in flash memory enclose your HTML code between 
// F(R"=====( HTML code here )=====");
// If you have 1 reading then you probably have 2 literal strings
// If you have 2 readings then you probably have 3 literal strings etc.

String homePagePart = F(R"=====(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>NoyZee</title>
   
</head>
<body>
    <button id="test" onclick="test()">test</button> 

<br>
    
</body>
<script >
    function test()
{
   return 10; 
}
</script>
</html> )=====")
/*String homePagePart2 = F(R"=====(
        </td>
         <td>Degree Cel</td>
        </tr>
      </table>
     </div>
	</body>
</html>
)=====");
*/