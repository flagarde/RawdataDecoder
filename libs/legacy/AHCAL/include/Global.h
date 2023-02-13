extern int   int_tmp;
extern char  char_tmp[200];
const int    channel_FEE        = 73;  //(36charges+36times + BCIDs )*16column+ ChipID
const int    cell_SP            = 16;
const int    chip_No            = 9;
const int    channel_No         = 36;
const int    Layer_No           = 40;
const double _Pos_X[channel_No] = {100.2411,  100.2411,  100.2411,  59.94146,  59.94146,  59.94146,  19.64182,  19.64182,  19.64182,  19.64182,  59.94146,  100.2411,  100.2411,  59.94146,  19.64182,  100.2411,  59.94146,  19.64182,
                                   -20.65782, -60.95746, -101.2571, -20.65782, -60.95746, -101.2571, -101.2571, -60.95746, -20.65782, -20.65782, -20.65782, -20.65782, -60.95746, -60.95746, -60.95746, -101.2571, -101.2571, -101.2571};
const double _Pos_Y[channel_No] = {141.04874, 181.34838, 221.64802, 141.04874, 181.34838, 221.64802, 141.04874, 181.34838, 221.64802, 261.94766, 261.94766, 261.94766, 302.2473,  302.2473,  302.2473,  342.54694, 342.54694, 342.54694,
                                   342.54694, 342.54694, 342.54694, 302.2473,  302.2473,  302.2473,  261.94766, 261.94766, 261.94766, 221.64802, 181.34838, 141.04874, 221.64802, 181.34838, 141.04874, 221.64802, 181.34838, 141.04874};
const double chip_dis_X         = 239.3;
const double chip_dis_Y         = 241.8;
const double HBU_X              = 239.3;
const double HBU_Y              = 725.4;
void         decode_cellid(int cellID, int& layer, int& chip, int& channel)
{
  layer   = cellID / 1E5;
  chip    = (cellID - layer * 1E5) / 1E4;
  channel = cellID % 100;
}
double Pos_X(int channel_ID, int chip_ID, int HBU_ID = 0)
{
  HBU_ID  = chip_ID / 3;
  chip_ID = chip_ID % 3;
  return (_Pos_Y[channel_ID] - chip_ID * chip_dis_Y);
}
double Pos_Y(int channel_ID, int chip_ID, int HBU_ID = 0)
{
  HBU_ID = chip_ID / 3;
  return -(-_Pos_X[channel_ID] + (HBU_ID - 1) * HBU_X);
}
