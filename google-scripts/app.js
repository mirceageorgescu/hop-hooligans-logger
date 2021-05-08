function doGet(e) { 
  Logger.log( JSON.stringify(e) );  // view parameters
  var result = 'Ok'; // assume success
  if (e.parameter == 'undefined') {
    result = 'No Parameters';
  }
  else {
    var sheet_id = '1LYgk3DTioQEZ8bXRQ_FzgyuSjljQnK51Pbf__i4cEMY';    // Spreadsheet ID
    var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();   // get Active sheet
    var newRow = sheet.getLastRow() + 1;            
    var rowData = [];
    rowData[0] = Utilities.formatDate(new Date(), "GMT+3", "MMM dd, YYYY") + ' at ' + Utilities.formatDate(new Date(), "GMT+3", "hh:mm:ss a"); // Timestamp in column A
    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);
      switch (param) {
        case 'sensor': //Parameter
          rowData[1] = value;
          result += ', Written sensor';
          break;
        case 'temperature': //Parameter
          rowData[2] = value; //Value in column B
          result += ', Written temperature';
          break;
        case 'humidity': //Parameter
          rowData[3] = value; //Value in column C
          result += ', Written humidity';
          break;
        case 'battery': //Parameter
          rowData[4] = value; //Value in column D
          result += ', Written battery';
          break;  
        default:
          result = +"unsupported parameter";
      }
    }
    Logger.log(JSON.stringify(rowData));
    // Write new row below
    var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
    newRange.setValues([rowData]);
  }
  // Return result of operation
  return ContentService.createTextOutput(result);
}
/**
* Remove leading and trailing single or double quotes
*/
function stripQuotes( value ) {
  return value.replace(/^["']|['"]$/g, "");
}