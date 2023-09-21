/*

  schuman.c(beta)

  Simple Schuman Resonance Oscillator

  (C)2015 https://github.com/ScrapWare

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/soundcard.h>
#include <sys/stat.h>

#define AppName "schuman\0"
#define AppVersion "0.72b\0"
#define AppAuthor "ScrapWare\0"
#define AppSupport "https://github.com/ScrapWare/\0"

#define AppUsage \
  "%s: Schuman resonance oscillator\n" \
  "\n" \
  "    -o  output filename(default is /dev/dsp)\n" \
  "    -s  output seconds(1-600)\n" \
  "    -l  output level(volume 1-99)\n" \
  "\n" \
  "Version: %s (C)2015 %s - %s\n\0"

#define AppDevice "/dev/dsp\0"

/* Solfeggio Frequencies ソルフェジオ周波数
 音		周波数	数秘パワー	意味
 UT：ウートゥ	396Hz	9		罪の意識と恐怖の解放
 RE：レ		417Hz	3		状況回復、変容の促進
 MI：ミ		528Hz	6		DNAの修復、奇跡
 FA：ファ	639Hz	9		関係性、つながり
SOL：ソ		741Hz	3		表現、解決
 LA：ラ		852Hz	6		直感の覚醒*/
#define sol1w 396.00
#define sol2w 417.00
#define sol3w 528.00
#define sol4w 639.00
#define sol5w 741.00
#define sol6w 852.00

/* O3 Ozone Frequency */
#define o3ofw  78.38

/* schuman base waves at 7.38hz */

/* from wikipedia en
#define sch1w   7.83
#define sch2w  14.30
#define sch3w  20.80
#define sch4w  27.30
#define sch5w  33.80
*/

/* from wikipedia jp
#define sch1w   7.83
#define sch2w  14.10
#define sch3w  20.30
*/

/* um?? oops!?
7.83, 14, 20, 26, 33, 39 and 45 Hz
*/

#define sch1w   7.83
#define sch2w  14.38
#define sch3w  20.83
#define sch4w  27.38
#define sch5w  33.83
#define sch6w  39.38
#define sch7w  45.83

#define SQ 2.0*M_PI
#define SP 0.293255131965

#define SD 1.00/(M_PI*M_PI)

void eprinte(const char *msg){
  fprintf(stderr, "%s: %s\n", AppName, msg);
}

int schuman_dsp_loop(unsigned short sb, unsigned int sr, unsigned short ch, double mv){

  int dsp;

  double qv = (pow(2.0, (double)sb)/2.0)*mv;

  double s1v = 1.00;
  double s2v = SD*8;
  double s3v = SD*6;
  double s4v = SD*4;
  double s5v = SD*2;
  double s6v = SD;
  double s7v = SD;

  signed short d; unsigned char c; int fm;

  unsigned long i;

  if((dsp = open(AppDevice, O_WRONLY)) == -1){
    eprinte("cannot open device.");
    return 1;
  }

  fm =  (sb == 8)? AFMT_U8 : AFMT_S16_LE;
  /*ioctl(dsp, SNDCTL_DSP_SETFMT, &fm);
  ioctl(dsp, SNDCTL_DSP_SPEED, &sr);
  ioctl(dsp, SNDCTL_DSP_STEREO, &ch);*/
  ioctl(dsp, SOUND_PCM_SETFMT, &fm);
  ioctl(dsp, SOUND_PCM_WRITE_BITS, &sb);
  ioctl(dsp, SOUND_PCM_WRITE_RATE, &sr);
  ioctl(dsp, SOUND_PCM_WRITE_CHANNELS, &ch);

  if(sb == 8){
    for(i=0;;i++){
      c = (unsigned char)(qv*((
        //sin(SQ*(sch1w)*i/sr) + (s2v*sin(SQ*sch2w*i/sr)) + (s3v*sin(SQ*sch3w*i/sr)) + (s4v*sin(SQ*sch4w*i/sr)) + (s5v*sin(SQ*sch5w*i/sr)) + (s6v*sin(SQ*sch6w*i/sr)) + (s7v*sin(SQ*sch7w*i/sr))
        //sin(SQ*(sch1w)*i/sr) + sin(SQ*sch2w*i/sr) + sin(SQ*sch3w*i/sr) + sin(SQ*sch4w*i/sr) + sin(SQ*sch5w*i/sr) + sin(SQ*sch6w*i/sr) + sin(SQ*sch7w*i/sr)
        sin(SQ*sch1w*i/sr) + (s2v*sin(SQ*sch2w*i/sr)) + (s3v*sin(SQ*sch3w*i/sr)) + (s4v*sin(SQ*sch4w*i/sr))
      )/4)+128); write(dsp, &c, sizeof(char));
    }
  } else{
    for(i=0;;i++){
      d = (signed short)round(qv*((
        //sin(SQ*(sch1w)*i/sr) + (s2v*sin(SQ*sch2w*i/sr)) + (s3v*sin(SQ*sch3w*i/sr)) + (s4v*sin(SQ*sch4w*i/sr)) + (s5v*sin(SQ*sch5w*i/sr)) + (s6v*sin(SQ*sch6w*i/sr)) + (s7v*sin(SQ*sch7w*i/sr))
        //sin(SQ*(sch1w)*i/sr) + sin(SQ*sch2w*i/sr) + sin(SQ*sch3w*i/sr) + sin(SQ*sch4w*i/sr) + sin(SQ*sch5w*i/sr) + sin(SQ*sch6w*i/sr) + sin(SQ*sch7w*i/sr)
        sin(SQ*sch1w*i/sr) + (s2v*sin(SQ*sch2w*i/sr)) + (s3v*sin(SQ*sch3w*i/sr)) + (s4v*sin(SQ*sch4w*i/sr))
      )/4)); write(dsp, &d, sizeof(signed short));
    }
  }

  close(dsp);

  return 0;

}

int schuman_wav_outs(unsigned short sb, unsigned int sr, unsigned short ch, double mv, unsigned short os, char *of){

  char wh[45];

  int dev;

  double qv = (pow(2.0, (double)sb)/2.0)*mv;

  unsigned short bs = sb/8;
  unsigned short ba = bs*ch;
  unsigned int bl = sr*ba;
  unsigned int ws = bl*os;
  unsigned int fs = ws+36;
  unsigned int lc = sr*os;

  unsigned int pf = 16;
  unsigned int pt = 1;

  double s1v = 1.00;
  double s2v = SD*8;
  double s3v = SD*6;
  double s4v = SD*4;
  double s5v = SD*2;
  double s6v = SD;
  double s7v = SD;

  signed short d; unsigned char c; int fm;

  unsigned long i;

  char *mp = (char*)memset((char*)wh, 0, 45);

  fprintf(stdout,
    "SampleBits: %d\n"
    "SampleRate: %d\n"
    "  Channels: %d\n"
    "BlockAlign: %d\n"
    " BlockSize: %d\n"
    "\n"
    "  PlayTime: %d(%d)\n"
    "PlayVolume: %d%%(%d)\n"
    "\n"
    , sb, sr, ch, ba, bl, os, ws, (int)(mv*100), (int)qv
  );

  if((dev = open(of, O_WRONLY|O_TRUNC|O_CREAT, 0644)) == -1){
    eprinte("cannot open output target.");
    return 1;
  }

  if(strcmp(of, "/dev/dsp") == 0){
    ioctl(dev, SOUND_PCM_SETFMT, AFMT_S16_LE);
    ioctl(dev, SOUND_PCM_WRITE_BITS, &sb);
    ioctl(dev, SOUND_PCM_WRITE_RATE, &sr);
    ioctl(dev, SOUND_PCM_WRITE_CHANNELS, &ch);
  }

  /* WAVE Header Direct */
  write(dev, "RIFF", 4);
  write(dev, &fs, 4);
  write(dev, "WAVEfmt ", 8);
  write(dev, &pf, 4);
  write(dev, &pt, 2);
  write(dev, &ch, 2);
  write(dev, &sr, 4);
  write(dev, &bl, 4);
  write(dev, &ba, 2);
  write(dev, &sb, 2);
  write(dev, "data", 4);
  write(dev, &ws, 4);

  if(sb == 8){
    for(i=0;i<lc;i++){
      c = (unsigned char)(qv*((
        //sin(SQ*(sch1w)*i/sr) + (s2v*sin(SQ*sch2w*i/sr)) + (s3v*sin(SQ*sch3w*i/sr)) + (s4v*sin(SQ*sch4w*i/sr)) + (s5v*sin(SQ*sch5w*i/sr)) + (s6v*sin(SQ*sch6w*i/sr)) + (s7v*sin(SQ*sch7w*i/sr))
        //sin(SQ*(sch1w)*i/sr) + sin(SQ*sch2w*i/sr) + sin(SQ*sch3w*i/sr) + sin(SQ*sch4w*i/sr) + sin(SQ*sch5w*i/sr) + sin(SQ*sch6w*i/sr) + sin(SQ*sch7w*i/sr)
        sin(SQ*sch1w*i/sr) + (s2v*sin(SQ*sch2w*i/sr)) + (s3v*sin(SQ*sch3w*i/sr)) + (s4v*sin(SQ*sch4w*i/sr))
      )/4)+128); write(dev, &c, sizeof(char));
    }
  } else{
    for(i=0;i<lc;i++){
      d = (signed short)round(qv*((
        //sin(SQ*(sch1w)*i/sr) + (s2v*sin(SQ*sch2w*i/sr)) + (s3v*sin(SQ*sch3w*i/sr)) + (s4v*sin(SQ*sch4w*i/sr)) + (s5v*sin(SQ*sch5w*i/sr)) + (s6v*sin(SQ*sch6w*i/sr)) + (s7v*sin(SQ*sch7w*i/sr))
        //sin(SQ*(sch1w)*i/sr) + sin(SQ*sch2w*i/sr) + sin(SQ*sch3w*i/sr) + sin(SQ*sch4w*i/sr) + sin(SQ*sch5w*i/sr) + sin(SQ*sch6w*i/sr) + sin(SQ*sch7w*i/sr)
        sin(SQ*sch1w*i/sr) + (s2v*sin(SQ*sch2w*i/sr)) + (s3v*sin(SQ*sch3w*i/sr)) + (s4v*sin(SQ*sch4w*i/sr))
      )/4)); write(dev, &d, sizeof(signed short));
    }
  }

  close(dev);

  return 0;

}

int file_exists(char *fn){

  struct stat st;

  return ((stat(fn, &st) == 0)? 1 : 0);

}

int main(int argc, char *argv[], char **envp){

            char *of = NULL;
  unsigned short  os = 0;
          double  ov = 0.75;

            char  c;
             int  i;

  for(i=0;i<argc;i++){
    if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "/h") == 0){
      printf(AppUsage, AppName, AppVersion, AppAuthor, AppSupport);
      return 0;
    } else if((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "/o") == 0) && i < (argc-1)){
      of = argv[(i+1)]; if(file_exists(of)){
        printf("%s", "output file path is exists, overwrite?(Y/N):");
          c = getc(stdin); if(c != 'y' && c != 'Y'){
          printf("bye!\n"); return 0;
        }
      }; i++; continue;
    } else if((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "/s") == 0) && i < (argc-1)){
      os = (unsigned short)atoi(argv[(i+1)]); if(os < 1 || os > 600){
        printf("072 request seconds was too strange.\n"); return 0;
      }; i++; continue;
    } else if((strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "/l") == 0) && i < (argc-1)){
      ov = (double)atoi(argv[(i+1)]); if(ov < 1.0 || ov > 99.0){
        printf("072 request volume was too strange.\n"); return 0;
      }; ov /= 100; i++; continue;
    }
  }

  if(of != NULL && os != 0){
    return schuman_wav_outs(16, 44100, 1, ov, os, of);
  } else{
    return schuman_dsp_loop(16, 44100, 1, ov);
  }

}
