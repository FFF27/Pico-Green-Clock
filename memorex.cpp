/* $TITLE=decode_ir_command() */
/* $PAGE */
/* ------------------------------------------------------------------ *\
             Function to decode an infrared command
                received using Memorex protocol.
                  Memorex protocol white paper:
                      Carrier: (37.9 kHz)
      1 Start bit made of:
             4500 micro-seconds Low  level / 4500 micro-seconds High level

      32 Data bits made of:
             bit 0 = 550 micro-seconds Low /   550 micro-seconds High
             bit 1 = 550 micro-seconds Low /  1700 micro-seconds High

    NOTE: All "brand-related" parameters must be kept in this function.
          This allows to replace only this include file with another
          "remote control brand" include file to support more remote
          controls while keeping the rest of the firmware untouched.
\* ------------------------------------------------------------------ */
UINT8 decode_ir_command(UCHAR FlagDebug, UINT8 *IrCommand)
{
  UCHAR FlagError;    // indicate an error in remote control packet received.
  UCHAR String[128];

  UINT8 CommandSubSection;  // subcommand for Set and Display commands.
  
  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;

  UINT64 DataBuffer;


  /* Initialization. */
  CommandSubSection = 0;  // used to jump directly to a specific "subsection" of commands "Set" or "Display".
  DataBuffer        = 0;  // data stream received from IR remote control.
  *IrCommand        = 0;  // initialize as zero on entry.
  FlagError         = FLAG_OFF;


  /* If number of state changes is not standard, display it when debug On. */
  if (DebugBitMask & DEBUG_IR_COMMAND)
  {
    if (IrStepCount != 73)
    {
      sprintf(String, "1) [%u]", IrStepCount);
      uart_send(__LINE__, String);
    }
  }



  /* Analyse and process each step change in remote control burst. */
  for (Loop1UInt16 = 0; Loop1UInt16 < IrStepCount; ++Loop1UInt16)
  {
    if (DebugBitMask & DEBUG_IR_COMMAND)
    {
      sprintf(String, "2) [%2.2u] %c %u", Loop1UInt16, Level[Loop1UInt16], ResultValue[Loop1UInt16]);
      uart_send(__LINE__, String);
    }

    /* Skip the two "Get ready" levels received from remote control
       (first: low level, then high level, both 4500 micro-seconds). */
    if (Loop1UInt16 < 2) continue;


    /* When reading a value that makes no sense, assume that we passed the last valid value of the IR stream. */
    if (ResultValue[Loop1UInt16] > 10000)
    {
      /* We reached end of data stream, get out of "for" loop. */
      break;
    
      if (DebugBitMask & DEBUG_IR_COMMAND)
      {
        sprintf(String, "3) Result > 10000: %u", Loop1UInt16);
        uart_send(__LINE__, String);
      }
    }


    if (Level[Loop1UInt16] == 'L')
    {
      /* Low level, it is a first half bit. Make a rough validation only. */
      if ((ResultValue[Loop1UInt16] < 400) || (ResultValue[Loop1UInt16] > 650))
      {
        FlagError = FLAG_ON;
      }
    }
    else
    {
      /* High level, it is a second half bit... assume it is a "zero" bit on entry. */
      DataBuffer <<= 1;

      /* Now check if our assumption was correct. */
      if ((ResultValue[Loop1UInt16] > 1600) && (ResultValue[Loop1UInt16] < 1800))
      {
        /* It was a "one" bit. DataBuffer has already been shifted left above, simply add 1 for current "one" bit. */
        ++DataBuffer;
      }


      if (DebugBitMask & DEBUG_IR_COMMAND)
      {
        sprintf(String, "4) Data: 0x%X", DataBuffer);
        uart_send(__LINE__, String);
      }
    }
  }


  /* Now that the command has been decoded, initalize variables to get ready for next burst decoding. */
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_READINGS; ++Loop1UInt16)
  {
    InitialValue[Loop1UInt16] = 0;
    FinalValue[Loop1UInt16]   = 0;
    ResultValue[Loop1UInt16]  = 0;
    Level[Loop1UInt16]        = 'X';
  }
  IrStepCount = 0;  // reset IrStepCount.


  if (DebugBitMask & DEBUG_IR_COMMAND)
  {
    sprintf(String, "5) Data: 0x%8.8X", DataBuffer);
    uart_send(__LINE__, String);
  }




  switch (DataBuffer)
  {
    case (0x2525609F):
      /* Button "Power". */
      /* Not used... as long as original Memorex equipment is Off, there
         is no chance that the remote used with the Green clock will interfere with it. */
      /// scroll_string(24, "Button <Power>");
    break;

    case (0x25257887):
      /* Button "CD door". */
      /// scroll_string(24, "Button <CD door>");
      *IrCommand = IR_DISPLAY_OUTSIDE_TEMP;
    break;

    case (0x2525807F):
      /* Button "1". */
      /// scroll_string(24, "Button <1>");
      CommandSubSection = 1;
    break;

    case (0x2525906F):
      /* Button "2". */
      /// scroll_string(24, "Button <2>");
      CommandSubSection = 2;
    break;

    case (0x25258877):
      /* Button "3". */
      /// scroll_string(24, "Button <3>");
      CommandSubSection = 3;
    break;

    case (0x25259867):
      /* Button "4". */
      /// scroll_string(24, "Button <4>");
      CommandSubSection = 4;
    break;

    case (0x252540BF):
      /* Button "5". */
      /// scroll_string(24, "Button <5>");
      CommandSubSection = 5;
    break;

    case (0x252550AF):
      /* Button "6". */
      /// scroll_string(24, "Button <6>");
      CommandSubSection = 6;
    break;

    case (0x252548B7):
      /* Button "7". */
      /// scroll_string(24, "Button <7>");
      CommandSubSection = 7;
    break;

    case (0x252558A7):
      /* Button "8". */
      /// scroll_string(24, "Button <8>");
      CommandSubSection = 8;
    break;

    case (0x2525C03F):
      /* Button "9". */
      /// scroll_string(24, "Button <9>");
      CommandSubSection = 9;
    break;

    case (0x2525D02F):
      /* Button "0". */
      /// scroll_string(24, "Button <0>");
      CommandSubSection = 10;
    break;

    case (0x2525C837):
      /* Button "Over". */
      /// scroll_string(24, "Button <Over>");
      *IrCommand = IR_DISPLAY_EVENTS_TODAY;
    break;

    case (0x252505FA):
      /* Button "Mute". */
      /// scroll_string(24, "Button <Mute>");
      *IrCommand = IR_DISPLAY_EVENTS_THIS_WEEK;
    break;

    case (0x252530CF):
      /* Button "Stop". */
      /// scroll_string(24, "Button <Stop>");
      *IrCommand = IR_BUTTON_BOTTOM_LONG;
    break;

    case (0x252520DF):
      /* Button "Play / Pause". */
      /// scroll_string(24, "Button <Play / Pause>");
      *IrCommand = IR_FULL_TEST;
    break;

    case (0x2525B04F):
      /* Button "Rewind / Down". */
      /// scroll_string(24, "Button <Rewind / Down>");
      *IrCommand = IR_IDLE_TIME;
    break;

    case (0x2525A05F):
      /* Button "Fast forward / Up". */
      scroll_string(24, "Button <Fast forward / Up>");
      /// *IrCommand = IR_NOT_USED_FOR_NOW;
    break;

    case (0x252504FB):
      /* Button "Volume up". */
      /// scroll_string(24, "Button <Volume up>");
      *IrCommand = IR_BUTTON_MIDDLE_QUICK;
    break;

    case (0x252506F9):
      /* Button "Volume down". */
      /// scroll_string(24, "Button <Volume down>");
      *IrCommand = IR_BUTTON_BOTTOM_QUICK;
    break;

    case (0x252538C7):
      /* Button "Random / Down". */
      /// scroll_string(24, "Button <Random / Down>");
      *IrCommand = IR_BUTTON_MIDDLE_LONG;
    break;

    case (0x2525D827):
      /* Button "Repeat / Up". */
      /// scroll_string(24, "Button <Repeat / Up>");
      *IrCommand = IR_BUTTON_TOP_LONG;
    break;

    case (0x252528D7):
      /* Button "Set / Memory / Clock". */
      /// scroll_string(24, "Button <Memory / Clock>");
      *IrCommand = IR_BUTTON_TOP_QUICK;
    break;

    case (0x2525708F):
      /* Button "Tuner". */
      /// scroll_string(24, "Button <Tuner>");
      *IrCommand = IR_DICE_ROLLING;
    break;

    case (0x25256897):
      /* Button "CD". */
      /// scroll_string(24, "Button <CD>");
      *IrCommand = IR_SILENCE_PERIOD;
    break;

    case (0x2525B847):
      /* Button "Time". */
      /// scroll_string(24, "Button <Time>");
      *IrCommand = IR_DISPLAY_SECOND;
    break;

    case (0x2525A857):
      /* Button "Display". */
      /// scroll_string(24, "Button <Display>");
      *IrCommand = IR_DISPLAY_GENERIC;
    break;

    default:
      /* Unrecognized. */
      if (DebugBitMask & DEBUG_IR_COMMAND)
      {
        sprintf(String, "Unrecognized IR command: 0x%X\r", DataBuffer);
        uart_send(__LINE__, String);
      }
      FlagError = FLAG_ON;
    break;
  }



  /* Given the large number of settings in the clock setup, remote control
     allows the user to switch to a specific "section" of the setups, for
     a much quicker access.
     Alarm setup and Timer setup only have a small number of settings,
     so there is no equivalent "direct section access" for those. */
  if (((CurrentClockMode == MODE_CLOCK_SETUP) || (CurrentClockMode == MODE_DISPLAY)) && (CommandSubSection > SETUP_CLOCK_LO_LIMIT) && (CommandSubSection < SETUP_CLOCK_HI_LIMIT))
  {
    /* Clock setup step definitions. 
       NOTE: Use the same definitions for "MODE_DISPLAY" as for "MODE_CLOCK_SETUP" for convenience.
             Master list is in the #define section at the beginning of Pico-Clock-Green.c */
    // SETUP_CLOCK_LO_LIMIT       0x00
    // SETUP_NONE                 0x00
    // SETUP_HOUR                 0x01
    // SETUP_MINUTE               0x02
    // SETUP_MONTH                0x03
    // SETUP_DAY_OF_MONTH         0x04
    // SETUP_YEAR                 0x05
    // SETUP_DST                  0x06
    // SETUP_KEYCLICK             0x07
    // SETUP_SCROLLING            0x08
    // SETUP_TEMP_UNIT            0x09
    // SETUP_LANGUAGE             0x0A
    // SETUP_TIME_FORMAT          0x0B
    // SETUP_HOURLY_CHIME         0x0C
    // SETUP_CHIME_TIME_ON        0x0D
    // SETUP_CHIME_TIME_OFF       0x0E
    // SETUP_NIGHT_LIGHT          0x0F
    // SETUP_NIGHT_LIGHT_TIME_ON  0x10
    // SETUP_NIGHT_LIGHT_TIME_OFF 0x11
    // SETUP_AUTO_BRIGHT          0x12
    // SETUP_CLOCK_HI_LIMIT       0x13

    /* House keeping for previous clock setting. */
    set_mode_out();
    
    /* Direct access to a specific clock setting. */
    switch (CommandSubSection)
    {
      case (1):
        /* Time, Date & Year settings (for Display, add display voltage). */
        SetupStep = 0x01;
      break;
          
      case (2):
        /* Daylight saving time. */
        SetupStep = 0x06;
      break;
          
      case (3):
        /* Beep (keyclick). */
        SetupStep = 0x07;
      break;
          
      case (4):
        /* Display scroll. */
        SetupStep = 0x08;
      break;
          
      case (5):
        /* Temperature unit. */
        SetupStep = 0x09;
      break;
          
      case (6):
        /* Language. */
        SetupStep = 0x0A;
      break;
          
      case (7):
        /* Time display format. */
        SetupStep = 0x0B;
      break;
        
      case (8):
        /* Hourly chime. */
        SetupStep = 0x0C;
      break;
          
      case (9):
        /* Night light. */
        SetupStep = 0x0F;
      break;
          
      case (10):
        /* Auto brightness. */
        SetupStep = 0x12;
      break;
    }
    
    
    if (CurrentClockMode == MODE_DISPLAY)
    {
      *IrCommand = IR_DISPLAY_GENERIC;
      // CurrentClockMode = MODE_DISPLAY;
      // SetupStep        = CommandSubSection;
      // FlagIdleCheck    = FLAG_ON;
    }
  }





  /***
  CurrentClockMode = MODE_TEST;
  sprintf(String, "1) CM:%u SS:%u IRC:%u", CommandSubSection, SetupStep, *IrCommand);
  scroll_string(24, String);
  // while (ScrollDotCount)
    sleep_ms(5000);
  ***/


  if (FlagError == FLAG_ON)
  {
    /***
    if (IrStepCount > 60)
    {
      /// Display DataBuffer only if it contains significant data.
      fill_display_buffer_4X7(0,  '(');
      fill_display_buffer_4X7(5,  '-');
      fill_display_buffer_4X7(10, '-');
      fill_display_buffer_4X7(15, '-');
      fill_display_buffer_4X7(20, ')');
    }
    ***/
    
    return 1;
  }
  else
  {
    sound_queue_active(50, 1);  // audible feedback when valid IR command has been decoded.
    sound_queue_active(50, SILENT);
    return 0;
  }
}
