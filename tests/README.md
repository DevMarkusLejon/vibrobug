# Tests

Current validation is firmware build plus hardware smoke testing.

Software checks:

```bash
pio run
```

Hardware checks:

1. Boot with motors disconnected and confirm AP starts.
2. Confirm `/status` returns JSON.
3. Connect drivers without motors and confirm PWM pins switch.
4. Connect motors with current-limited supply.
5. Send `/drive`, then stop commands and confirm timeout stops motors within 500 ms.

